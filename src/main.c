#include "../include/net/client.h"
#include "../include/net/server.h"
#include "../include/shared.h"
#include <stdbool.h>

int main(int argc, char **argv) {
  bool server = argc > 1 && str_eq(argv[1], "--server");

  GAME_SIDE = server ? SIDE_SERVER : SIDE_CLIENT;

  if (server) {
    if (argc == 4) {
      char *ip_addr = argv[2];
      char *port = argv[3];

      server_start(ip_addr, atoi(port));
    } else {
      fprintf(stderr, "Ip address and port need to be provided for the server\n");
      return 1;
    }
  } else {
    client_start();
  }
}
