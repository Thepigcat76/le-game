#include "../../include/net/server.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

ServerGame SERVER_GAME;

static pthread_mutex_t SERVER_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void server_init(void) { SERVER_GAME = (ServerGame){}; }

static void *server_game(void *args) {
  while (true) {
    printf("Game logic\n");
  }
  return NULL;
}

static void handle_connection(addr_t client_addr) {
  
}

static void *server_packet_listener(void *args) {
  PollClient fds[MAX_CLIENTS];
  while (true) {
    size_t client_addresses_amount;
    pthread_mutex_lock(&SERVER_MUTEX);
    {
      client_addresses_amount = SERVER_GAME.players;

      for (int i = 0; i < client_addresses_amount; i++) {
        addr_t s = SERVER_GAME.client_addresses[i];
        if (s == _INVALID_SOCKET || s == 0) {
          fprintf(stderr, "[DEBUG] Skipping invalid socket at index %d (fd=%lld)\n", i, (long long)s);
          fds[i].fd = _INVALID_SOCKET;
          fds[i].events = 0;
        } else {
          fds[i].fd = s;
          fds[i].events = POLLRDNORM;
        }
      }
    }
    pthread_mutex_unlock(&SERVER_MUTEX);

    int poll_result = sockets_server_poll_clients(fds, client_addresses_amount, 100);

    sockets_server_handle_poll(poll_result);

    for (int i = 0; i < client_addresses_amount; i++) {
      if (fds[i].revents & POLLIN) {
        addr_t client_fd = fds[i].fd;

        handle_connection(client_fd);
      }
      if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        fprintf(stderr, "Client %d disconnected or error\n", fds[i].fd);
        sockets_close(fds[i].fd);
        // You may want to mark the player slot as disconnected
      }
    }
  }
  return NULL;
}

static void *server_player_listener(void *args) {
  addr_t server_addr = ((int *)args)[0];
  while (true) {
    int64_t client_fd = sockets_server_accept_client(server_addr);

    pthread_mutex_lock(&SERVER_MUTEX);
    {
      size_t player_id = SERVER_GAME.players++;
      SERVER_GAME.client_addresses[player_id] = client_fd;
      // TODO: Send packets to clients
    }
    pthread_mutex_unlock(&SERVER_MUTEX);
  }
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

  printf("Server finished\n");

  sockets_close(server_addr);
}
