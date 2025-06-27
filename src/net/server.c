#include "../../include/net/server.h"
#include <pthread.h>
#include <stdio.h>

static void *server_game(void *args) {
  while (1) {
    printf("Game logic\n");
  }
  return NULL;
}

static void *server_packet_listener(void *args) {
  return NULL;
}

static void *server_player_listener(void *args) {
  return NULL;
}

void server_start(const char *ip_addr, uint32_t port) {
  // Creates a socket for the server
  addr_t server_addr = sockets_open_server(ip_addr, port);

  if (server_addr < 0) {
    printf("Failed to open server\n");
    return;
  } else {
    printf("Launched server successfully!\n");
  }

  pthread_t game_thread;
  pthread_t packet_listener_thread;
  pthread_t player_listener_thread;

  addr_t args[] = {server_addr};
  if (pthread_create(&game_thread, NULL, server_game, args)) {
    printf("Failed to create game thread\n");
    return;
  }

  if (pthread_create(&packet_listener_thread, NULL, server_packet_listener, args)) {
    printf("Failed to create packet listener thread\n");
    return;
  }

  if (pthread_create(&player_listener_thread, NULL, server_player_listener, args)) {
    printf("Failed to create player listener thread\n");
    return;
  }

  pthread_join(game_thread, NULL);
  pthread_join(packet_listener_thread, NULL);
  pthread_join(player_listener_thread, NULL);

  sockets_close(server_addr);
}
