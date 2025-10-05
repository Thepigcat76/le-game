#include "../../include/net/server.h"
#include "../../include/server_ui.h"
#include "../../include/array.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <pthread.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

ServerGame SERVER_GAME = {0};
UiRenderer UI_RENDERER;

static pthread_mutex_t SERVER_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void server_init(ServerGame *game) {
}

static void calc_server_ui_height(UiRenderer *ui_renderer) {
  if (ui_renderer->ui_height == -1) {
    ui_renderer->cur_y = 0;
    ui_renderer->simulate = true;
    server_ui_render(ui_renderer, &SERVER_GAME);
    ui_renderer->ui_height = ui_renderer->cur_y;

    ui_renderer->simulate = false;
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
  }
}

static void save_save_data(const Save *save) {
  size_t loaded_saves_len = array_len(save->loaded_spaces);
  for (int i = 0; i < loaded_saves_len; i++) {
    space_save(save->descriptor, &save->loaded_spaces[i]);
  }
}

static void server_ui_setup_raylib(void) {
  InitWindow(400, 400, "Server UI");
  SetTargetFPS(60);
}

static void *server_game(void *args) {
  // Setup bump allocator for item containers
  _internal_item_container_init();

  // Setup raylib for the server ui
  // Make sure to call before any textures get loaded
  server_ui_setup_raylib();

  // init random
  shared_setup();

  // Init server game
  server_init(&SERVER_GAME);

  // Create and init common game
  Game _game = {0};
  Game *game = malloc(sizeof(Game));
  memcpy(game, &_game, sizeof(Game));
  game_init(game);
  game->server_game = &SERVER_GAME;
  SERVER_GAME.game = game;

  // Create ui renderer for server ui
  // And load button default textures
  UI_RENDERER = ui_renderer_new();

  // setup registries
  game_registry_setup();

  // setup tile_categories
  tile_categories_setup(game);

  // Reload common resources (creates them)
  game_reload(game);

  calc_server_ui_height(&UI_RENDERER);

  while (!WindowShouldClose()) {
    UI_RENDERER.cur_x = 0;
    UI_RENDERER.cur_y = 0;

    BeginDrawing();
    {
      ClearBackground(BLACK);
      server_ui_render(&UI_RENDERER, &SERVER_GAME);
    }
    EndDrawing();
  }

  save_save_data(&SERVER_GAME.game->cur_save);

  CloseWindow();
  exit(0);

  return NULL;
}

static void handle_connection(addr_t client_addr) {}

static void *server_packet_listener(void *args) {
  PollClient fds[MAX_CLIENTS];
  while (true) {
    size_t client_addresses_amount;
    pthread_mutex_lock(&SERVER_MUTEX);
    {
      client_addresses_amount = SERVER_GAME.clients_amount;

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
        //fprintf(stderr, "Client %d disconnected or error\n", fds[i].fd);
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
      size_t player_id = SERVER_GAME.clients_amount++;
      SERVER_GAME.client_addresses[player_id] = client_fd;
      // TODO: Send packets to clients
      printf("Player connected!\n");
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
