#include "rr_client_lib.h"

#include "uio.h"
#include "util.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include "rr_service.h"
#ifdef _WIN32
#include <WS2tcpip.h>
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <sys/types.h>
#define RR_MAX_MESSAGE_LEN 1024
// do_login sends a login message to the server in order to authenticate the
// user and give them a unique token
int do_login(int conn, const char *name, const char *pin, unsigned int *token) {
  // send login message to server
  char msg_buf[RR_MAX_USERNAME_LEN * 2 + 4];
  snprintf(msg_buf, sizeof(msg_buf), "REGISTER %s %s\n", name, pin);
  if (uio_write(msg_buf, strlen(msg_buf), conn) != strlen(msg_buf)) {
    perror("do_login: write error");
    return -1;
  }

  // get and read server response
  char response_buf[32];
  ssize_t response_len =
      uio_readline(response_buf, sizeof(response_buf) - 1, conn);

  // if theres a read error return
  if (response_len < 0) {
    perror("do_login: read error");
    return -1;
  }
  // add null terminator to the response buffer and get the token
  response_buf[response_len] = '\0';
  printf("Recieved response: %s\n", response_buf);

  char status[8];
  unsigned long parsed_value;
  int sscanf_res = sscanf(response_buf, "%7s %lu", status, &parsed_value);

  // if token cant get recieved return
  if (sscanf_res != 2 || strcmp(status, "ok") != 0 ||
      parsed_value == ULONG_MAX) {
    perror("do_login: token parsing error");
    return -1;
  }

  // set the token
  *token = (unsigned int)parsed_value;
  return 0;
}

// post to rhodes ramble
int do_post(int conn, unsigned int token, const char *text) {
  char msg_buf[RR_MAX_MESSAGE_LEN + 64];
  snprintf(msg_buf, sizeof(msg_buf), "%s %u %s\n", RR_POST, token, text);
  if (uio_write(msg_buf, strlen(msg_buf), conn) != strlen(msg_buf)) {
    perror("do_post: write error");
    return -1;
  }

  char response_buf[32];
  ssize_t response_len =
      uio_readline(response_buf, sizeof(response_buf) - 1, conn);
  if (response_len < 0) {
    perror("do_post: read error");
    return -1;
  }

  response_buf[response_len] = '\0';
  printf("Received response: %s\n", response_buf);
  if (strncmp(response_buf, "ok", 2) != 0) {
    fprintf(stderr, "do_post: post failed\n");
    return -1;
  }

  return 0;
}

// comment on a post
int do_comment(int conn, unsigned int token, unsigned int parent,
               const char *text) {
  char msg_buf[RR_MAX_MESSAGE_LEN + 64];
  snprintf(msg_buf, sizeof(msg_buf), "%s %u %u %s\n", RR_COMMENT, token, parent,
           text);

  if (uio_write(msg_buf, strlen(msg_buf), conn) != strlen(msg_buf)) {
    perror("do_comment: write error");
    return -1;
  }

  char response_buf[32];
  ssize_t response_len =
      uio_readline(response_buf, sizeof(response_buf) - 1, conn);

  if (response_len < 0) {
    perror("do_comment: read error");
    return -1;
  }

  response_buf[response_len] = '\0';
  printf("Received response: %s\n", response_buf);

  if (strncmp(response_buf, "ok", 2) != 0) {
    fprintf(stderr, "do_comment: comment failed\n");
    return -1;
  }

  return 0;
}

// upvote or downvote a post. problem: doesn't allow any other input after but
// still registers server side.
int do_vote(int conn, unsigned int token, unsigned int id, int up_down) {
  char vote_command[64];
  int ret;
  // formatting
  snprintf(vote_command, sizeof(vote_command), "VOTE %u %u %d", token, id,
           up_down);

  ret = uio_write(vote_command, strlen(vote_command), conn);
  if (ret != strlen(vote_command)) {
    perror("uio_write");
    return -1;
  }

  char response[64];
  memset(response, 0, sizeof(response));
  ret = uio_readline(response, sizeof(response) - 1, conn);

  if (ret < 0) {
    perror("uio_readline");
    return -1;
  }

  int success;
  if (sscanf(response, "VOTE %d", &success) != 1) {
    fprintf(stderr, "Error: Invalid server response: %s\n", response);
    return -1;
  }

  if (success == 0) {
    fprintf(stderr, "Error: Vote failed\n");
    return -1;
  }

  return 0;
}

// gets posts made to rhodes ramble. problem: couldn't get it to work with
// uio_readline
int get_posts(int conn, unsigned int token, struct rr_client_post_t **posts,
              int posts_len) {
  fflush(stdin);
  // formatting
  char msg_buf[64];
  snprintf(msg_buf, sizeof(msg_buf), "%s %u\n", RR_GET, token);

  if (uio_write(msg_buf, strlen(msg_buf), conn) != strlen(msg_buf)) {
    perror("get_posts: write error");
    return -1;
  }

  char response_buf[RR_MAX_POSTS * (RR_MAX_MESSAGE_LEN + 64)] = {0};
  ssize_t response_len = read(conn, response_buf, sizeof(response_buf) - 1);

  printf("server response: %s\n", response_buf);
  if (response_len < 0) {
    perror("get_posts: read error");
    return -1;
  }

  // iterate through responses and print
  char *line = strtok(response_buf, "\n");
  int num_posts = 0;

  while (line && num_posts < posts_len) {
    struct rr_client_post_t *post = malloc(sizeof(struct rr_client_post_t));
    post->text = malloc(RR_MAX_MESSAGE_LEN + 1);
    int parsed = sscanf(line, "%u %lc %d %[^\n]", &post->id, &post->icon,
                        &post->karma, post->text);

    if (parsed == 4) {
      posts[num_posts++] = post;
      printf("parsed post: id=%u, icon=%lc, karma=%d, text=%s\n", post->id,
             post->icon, post->karma, post->text);
    } else {
      free(post->text);
      free(post);
      break;
    }

    line = strtok(NULL, "\n");
  }

  if (num_posts < posts_len) {
    posts[num_posts] = NULL;
  }

  return num_posts;
}

// gets comments. problem: same issue as upvoting and downvoting
int get_comments(int conn, unsigned int token, unsigned int parent,
                 struct rr_client_post_t **posts, int posts_len) {
  char msg_buf[RR_MAX_MESSAGE_LEN + 64];
  snprintf(msg_buf, sizeof(msg_buf), "%s %u %u\n", RR_COMMENTS, token, parent);

  if (uio_write(msg_buf, strlen(msg_buf), conn) != strlen(msg_buf)) {
    perror("get_comments: write error");
    return -1;
  }

  int num_comments = 0;
  char line_buf[RR_MAX_MESSAGE_LEN + 64];
  while (uio_readline(line_buf, sizeof(line_buf) - 1, conn) &&
         num_comments < posts_len) {
    printf("%s\n", line_buf);
    struct rr_client_post_t *post = malloc(sizeof(struct rr_client_post_t));
    post->text = malloc(RR_MAX_MESSAGE_LEN + 1);
    int parsed = sscanf(line_buf, "%u %lc %d %[^\n]", &post->id, &post->icon,
                        &post->karma, post->text);

    if (parsed == 4) {
      posts[num_comments++] = post;
      printf("parsed comment: id=%u, icon=%lc, karma=%d, text=%s\n", post->id,
             post->icon, post->karma, post->text); // Print parsed comment

    } else {
      free(post->text);
      free(post);
    }
  }
  printf("hello");
  if (num_comments < posts_len) {
    posts[num_comments] = NULL;
  }
  return num_comments;
}

// gets users info
int get_user_info(int conn, unsigned int token,
                  struct rr_client_user_info_t *user) {
  char msg_buf[64];
  snprintf(msg_buf, sizeof(msg_buf), "%s %u\n", RR_USER_INFO, token);
  if (uio_write(msg_buf, strlen(msg_buf), conn) != strlen(msg_buf)) {
    perror("get_user_info: write error");
    return -1;
  }

  char response_buf[64];
  ssize_t response_len =
      uio_readline(response_buf, sizeof(response_buf) - 1, conn);

  if (response_len < 0) {
    perror("get_user_info: read error");
    return -1;
  }
  // read and print the info
  response_buf[response_len] = '\0';
  wchar_t icon;
  int karma;
  if (sscanf(response_buf, "%lc %d", &icon, &karma) == 2) {
    user->icon = icon;
    user->karma = karma;
    return 0;
  } else {
    return -1;
  }
}

void free_posts(struct rr_client_post_t **posts, int posts_len) {
  for (int i = 0; i < posts_len && posts[i]; i++) {
    free(posts[i]->text);
    free(posts[i]);
  }
}
