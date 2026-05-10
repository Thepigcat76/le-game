#include "../../include/array.h"
#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/log.h"
#include "../../include/net/client.h"
#include <math.h>
#include <raylib.h>

#define BREAK_PROGRESS_FRAMES 6

static void game_render_break_progress(ClientGame *client, TilePos break_pos, int break_time, int break_progress) {
  if (break_progress != -1) {
    int index = floor_div(break_progress, break_time / BREAK_PROGRESS_FRAMES);
    DrawTextureRec(client->texture_manager.textures[TEXTURE_BREAK], rectf(0, index * TILE_SIZE, TILE_SIZE, TILE_SIZE),
                   vec2f(break_pos.x * TILE_SIZE, break_pos.y * TILE_SIZE), WHITE);
    TraceLog(LOG_DEBUG, "Texture index: %d", index);
  }
}

static void client_render_beings(ClientGame *client) {
  for (int i = 0; i < array_len(client->world->beings); i++) {
    if (CheckCollisionPointRec(GetMousePosition(), client->world->beings[i].context.box)) {
      client->hovered_being = &client->world->beings[i];
    }
    being_render(&client->world->beings[i]);
  }
}

void client_world_render(ClientGame *client, float alpha) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, CLIENT_PLAYER->cam);

  world_render_layer(client->world, TILE_LAYER_GROUND);

  world_render_layer_top_split(client->world, CLIENT_PLAYER->box, true);

  client_render_beings(client);

  client_render_particles(client, true);

  player_render(CLIENT_PLAYER, alpha);

  bool zoom_in = IS_KEY_DOWN(zoom_in);
  bool zoom_out = IS_KEY_DOWN(zoom_out);

  player_handle_zoom(CLIENT_PLAYER, zoom_in, zoom_out, alpha);

  world_render_layer_top_split(client->world, CLIENT_PLAYER->box, false);

  if (CLIENT_PLAYER != NULL) {
    // log_debug("Slay");
    if (CLIENT_PLAYER->break_tile != NULL) {
      game_render_break_progress(client, CLIENT_PLAYER->break_tile_pos, CLIENT_PLAYER->break_tile->type->tile_props.break_time,
                                 CLIENT_PLAYER->break_progress);
    }
  }

  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);
  client->hovered_tile = world_highest_tile_at(client->world, vec2i(x_index, y_index));
  Rectangle rec = (Rectangle){.x = x_index * (TILE_SIZE), .y = y_index * (TILE_SIZE), .width = (TILE_SIZE), .height = (TILE_SIZE)};
  bool slot_selected = client->slot_selected;
  bool interaction_in_range = abs((int)CLIENT_PLAYER->box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)CLIENT_PLAYER->box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  if (!slot_selected && interaction_in_range) {
    rec_draw_outline(rec, BLUE);
    bool can_break = item_tool_correct_for_tile(&client->cur_player.held_item, client->hovered_tile, &client->game.tile_category_lookup);
    DrawTexture(client->texture_manager.textures[can_break ? TEXTURE_OK : TEXTURE_ERR], rec.x + 4, rec.y - 8, WHITE);
  }

#ifdef DEBUG_BUILD
  debug_render(&client->game.debug);
#endif
}

void client_render(ClientGame *client, float alpha) {
  client->ui_renderer.cur_x = 0;
  client->ui_renderer.cur_y = 0;

  BeginDrawing();
  {
    ClearBackground(DARKGRAY);

    Vec2f mouse_pos = GetMousePosition();
    if (client->world != NULL) {
      Camera2D *cam = &CLIENT_PLAYER->cam;

      Vector2 mouse_world_pos = GetScreenToWorld2D(mouse_pos, *cam);
      Vector2 light_pos = {(mouse_pos.x / GetScreenWidth()), 1.0 - (mouse_pos.y / GetScreenHeight())};

      Vector3 light_color = {1.0f, 1.0f, 0.8f}; // warm white
      float light_radius = 0;

      if (CLIENT_PLAYER != NULL) {
        light_radius = CLIENT_PLAYER->held_item.type.item_props.light_source ? 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime())) : 0;
      }

      ShaderVarLookupLighting lighting_lookup = client->shader_manager.lookups[SHADER_LIGHTING].var.lighting;
      Shader lighting_shader = client->shader_manager.shaders[SHADER_LIGHTING];

      SetShaderValue(lighting_shader, lighting_lookup.light_pos_loc, &light_pos, SHADER_UNIFORM_VEC2);
      SetShaderValue(lighting_shader, lighting_lookup.light_color_loc, &light_color, SHADER_UNIFORM_VEC3);
      SetShaderValue(lighting_shader, lighting_lookup.light_radius_loc, &light_radius, SHADER_UNIFORM_FLOAT);
      SetShaderValue(lighting_shader, lighting_lookup.ambient_light_loc, &CONFIG.ambient_light, SHADER_UNIFORM_FLOAT);
      BeginTextureMode(client->world_texture);
      {
        BeginMode2D(*cam);
        {
          ClearBackground(DARKGRAY);
          if (!client_menu_hides_game(client, client->cur_menu)) {
            log_debug("Rendering client world");
            client_world_render(client, alpha);

            // TODO: MOVE TO GAME RENDER FUNCTION

            client_render_particles(client, false);

            if (IsKeyReleased(KEYBINDS.open_close_backpack_menu_key)) {
              // game_set_menu(game, MENU_BACKPACK);
            }

            // CAMERA END
          }
        }
        EndMode2D();
      }
      EndTextureMode();

      if (!client_menu_hides_game(client, client->cur_menu)) {
        // RENDER WORLD
        BeginShaderMode(lighting_shader);
        {
          DrawTextureRec(client->world_texture.texture,
                         (Rectangle){0, 0, (float)client->world_texture.texture.width, -(float)client->world_texture.texture.height},
                         (Vector2){0, 0}, WHITE);
        }
        EndShaderMode();
      }

      if (!client_menu_hides_game(client, client->cur_menu)) {
        client_render_overlay(client);
      }
    }

    // Always render menus

    client_render_menu(client);

    bool can_cursor_interact_with_tile = cursor_can_interact_with_tile(client, client->hovered_tile);
    bool can_cursor_interact_with_being = cursor_can_interact_with_being(client, client->hovered_being);

    float scale = 3;
    DrawTextureEx(client->texture_manager
                      .textures[can_cursor_interact_with_tile || can_cursor_interact_with_being ? TEXTURE_CURSOR_FIST : TEXTURE_CURSOR],
                  (Vector2){.x = mouse_pos.x, .y = mouse_pos.y}, 0, scale, WHITE);

    if (client_menu_is_container(client, client->cur_menu) && !item_is_empty(&CLIENT_PLAYER->dragged_item)) {
      item_render(&CLIENT_PLAYER->dragged_item, mouse_pos.x - 22, mouse_pos.y - 22);
    } else if (can_cursor_interact_with_tile) {
      item_render(&CLIENT_PLAYER->held_item, mouse_pos.x - 22, mouse_pos.y - 22);
    }
  }

  EndDrawing();
}
