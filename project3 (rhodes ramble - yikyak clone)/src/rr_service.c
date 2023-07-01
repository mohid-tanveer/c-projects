#include "rr_service.h"

#include "users.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include "rr_core.h"
#include "uio.h"
#include <WS2tcpip.h>
#include <winsock2.h>
#include <sys/types.h>

void init_rr_service() { rr_service_init(); }

void destroy_rr_service() {}

void parse_and_dispatch(int response_fd, char *msg, int msg_len) {
  char command[msg_len + 1];
  int ret = sscanf(msg, "%s", command);
  if (ret != 1) {
    dprintf(response_fd, "0\n");
    return;
  }

  if (strcmp(command, RR_REGISTER) == 0) {
    char name[MAX_USERNAME_LEN + 1], pin[MAX_PIN_LEN + 1];
    ret = sscanf(msg, "%*s %s %s", name, pin);
    if (ret != 2) {
      dprintf(response_fd, "0\n");
      return;
    }

    unsigned int token = rr_register(name, atoi(pin));
    if (token != 0) {
      // registeration complete
      dprintf(response_fd, "%u\n", token);
    } else {
      // registration fail
      dprintf(response_fd, "0\n");
    }
  } else if (strcmp(command, RR_POST) == 0) {
    unsigned int token;
    char post_text[1024];
    ret = sscanf(msg, "%*s %u %[^\n]", &token, post_text);
    if (ret != 2) {
      dprintf(response_fd, "0\n");
      return;
    }
    unsigned int post_id = rr_add_post(token, post_text);
    dprintf(response_fd, "%u\n", post_id);
  } else if (strcmp(command, RR_COMMENT) == 0) {
    unsigned int token, post_id;
    char comment_text[1024];
    ret = sscanf(msg, "%*s %u %u %[^\n]", &token, &post_id, comment_text);
    if (ret != 3) {
      dprintf(response_fd, "0\n");
      return;
    }

    unsigned int comment_id = rr_add_comment(token, post_id, comment_text);
    dprintf(response_fd, "%u\n", comment_id);
  } else if (strcmp(command, RR_GET) == 0) {
    // get posts
    unsigned int token;
    sscanf(msg, "%*s %u", &token);
    struct rr_post **posts = NULL;
    int num_posts = rr_get_posts(token, &posts);
    if (num_posts != 1) {
      for (int i = 0; i < num_posts; i++) {
        dprintf(response_fd, "%u > (%d) %lc %s\n", posts[i]->post_id,
                posts[i]->karma, posts[i]->icon, posts[i]->text);
        free(posts[i]);
      }
      free(posts);
    } else {
      dprintf(response_fd, "-1\n");
    }
  } else if (strcmp(command, RR_COMMENTS) == 0) {
    unsigned int token, post_id;
    ret = sscanf(msg, "%*s %u %u", &token, &post_id);
    if (ret != 2) {
      dprintf(response_fd, "0\n");
      return;
    }
    struct rr_post **comments = NULL;
    int num_comments = rr_get_comments(token, post_id, &comments);
    if (num_comments != -1) {
      for (int i = 0; i < num_comments; i++) {
        dprintf(response_fd, "%u %lc %d %s\n", comments[i]->post_id,
                comments[i]->icon, comments[i]->karma, comments[i]->text);
        free(comments[i]);
      }
      free(comments);
    } else {
      dprintf(response_fd, "-1\n");
    }
  } else if (strcmp(command, RR_USER_INFO) == 0) {
    unsigned int token;
    ret = sscanf(msg, "%*s %u", &token);
    if (ret != 1) {
      dprintf(response_fd, "0\n");
      return;
    }

    struct rr_user user;
    if (rr_get_user_info(token, &user)) {
      dprintf(response_fd, "%lc %d\n", user.icon, user.karma);
    } else {
      dprintf(response_fd, "0\n");
    }
  } else if (strcmp(command, RR_VOTE) == 0) {
    unsigned int voter_token, post_id;
    int updown;
    ret = sscanf(msg, "%*s %u %u %d", &voter_token, &post_id, &updown);
    if (ret != 3) {
      dprintf(response_fd, "0\n");
      return;
    }
    int result = rr_vote(voter_token, post_id, updown);
    dprintf(response_fd, "%d\n", result);
  } else {
    dprintf(response_fd, "0\n");
  }
}
