#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#include <WS2tcpip.h>
#include <winsock2.h>
#include <sys/types.h>

#include "conn.h"
#include "rr_service.h"
#include "uio.h"

#define MAX_CONN 1024
#define BUFFER_SIZE 2048
#define HOST_PORT_LEN 256
#define BACKLOG 1024
#define RR_MSG_END "### END ###\n"

struct my_thread_args {
  int client_sockfd;
  pthread_mutex_t *mu;
};

// Handles a connection from a client; reads requests and delegates responding
// to the rr_service (via parse_and_dispatch).
void handle(void *argsP) {
  struct my_thread_args *args = (struct my_thread_args *)argsP;
  int client_sockfd = args->client_sockfd;
  pthread_mutex_t *mu = args->mu;
  char inbuff[BUFFER_SIZE];
  int msz;

  // read a line from the client until EOF.
  while (msz = uio_readline(inbuff, BUFFER_SIZE, client_sockfd), msz > 0) {
    // chop off the \n.
    inbuff[msz] = '\0';

    pthread_mutex_lock(mu);
    parse_and_dispatch(client_sockfd, inbuff, msz);
    pthread_mutex_unlock(mu);

    uio_write(RR_MSG_END, strlen(RR_MSG_END), client_sockfd);
  }

  // goodbye 🥹
  close(client_sockfd);
  free(argsP);
}

int main(int argc, char **argv) {
  srand(time(NULL)); // remove to seed with a fixed value
  // srand(42);
  setlocale(LC_ALL, "");

  // Initialize the service layer
  init_rr_service();

  // Ensure that we were given a port to listen on in argv[1]
  if (argc < 2) {
    fprintf(stderr,
            "Usage: %s PORT\n\nStarts Rhodes Ramble server listening on the "
            "given port PORT.\n",
            argv[0]);
    exit(1);
  }

  // TODO: create a server socket! (use conn.h functions)
  int conn = conn_listen(argv[1]);
  if (conn < 0) {
    fprintf(stderr, "Error listening on %s\n", argv[1]);
  }
  printf("listening on %s...\n", argv[1]);

  pthread_mutex_t mu;
  pthread_mutex_init(&mu, NULL);

  while (1) {
    socklen_t csz = sizeof(struct sockaddr_storage);
    struct sockaddr_storage *caddr = malloc(csz);

    // TODO: accept a connection from a client!
    int client = accept(conn, (struct sockaddr *)caddr, &csz);

    if (client < 0) {
      // handle accept error.
      perror("accept");
      free(caddr);
      continue;
    }

    struct my_thread_args *args = malloc(sizeof(struct my_thread_args));
    args->client_sockfd = client;
    args->mu = &mu;

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, (void *)handle, (void *)args);
    pthread_detach(thread_id);

    // clean up client resources.
    free(caddr);
  }

  return 0;
}
