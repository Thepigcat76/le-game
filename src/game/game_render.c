#include "../../include/config.h"
#include "../../include/game.h"
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

void client_world_render(ClientGame *client, float alpha) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, client->player->cam);

  world_render_layer(client->world, TILE_LAYER_GROUND);

  world_render_layer_top_split(client->world, &client->player, true);

  for (int i = 0; i < client->world->beings_amount; i++) {
    being_render(&client->world->beings[i]);
  }

  client_render_particles(client, true);

  player_render(client->player, alpha);

  bool zoom_in = IS_KEY_DOWN(zoom_in);
  bool zoom_out = IS_KEY_DOWN(zoom_out);

  player_handle_zoom(client->player, zoom_in, zoom_out, alpha);

  world_render_layer_top_split(client->world, &client->player, false);

  game_render_break_progress(client, client->player->break_tile_pos,
                             client->player->break_tile.type->tile_props.break_time, client->player->break_progress);

  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);
  Rectangle rec =
      (Rectangle){.x = x_index * (TILE_SIZE), .y = y_index * (TILE_SIZE), .width = (TILE_SIZE), .height = (TILE_SIZE)};
  bool slot_selected = client->slot_selected;
  bool interaction_in_range =
      abs((int)client->player->box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)client->player->box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  if (!slot_selected && interaction_in_range) {
    rec_draw_outline(rec, BLUE);
  }

#ifdef SURTUR_DEBUG
  debug_render();
#endif
}

void client_render(ClientGame *client, float alpha) {
  client->ui_renderer.cur_x = 0;
  client->ui_renderer.cur_y = 0;

  BeginDrawing();
  {
    ClearBackground(DARKGRAY);

    Vector2 mousePos = GetMousePosition();
    if (client->world != NULL) {
      Camera2D *cam = &client->player->cam;

      Vector2 mouse_world_pos = GetScreenToWorld2D(mousePos, *cam);
      Vector2 light_pos = {(mousePos.x / GetScreenWidth()), 1.0 - (mousePos.y / GetScreenHeight())};

      Vector3 light_color = {1.0f, 1.0f, 0.8f}; // warm white
      float light_radius =
          GAME.player->held_item.type.item_props.light_source ? 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime())) : 0;

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

          if (!client_cur_menu_hides_game(client)) {
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

      if (!client_cur_menu_hides_game(client)) {
        // RENDER WORLD
        BeginShaderMode(lighting_shader);
        {
          DrawTextureRec(client->world_texture.texture,
                         (Rectangle){0, 0, (float)client->world_texture.texture.width,
                                     -(float)client->world_texture.texture.height},
                         (Vector2){0, 0}, WHITE);
        }
        EndShaderMode();
      }

      if (client->player->held_item.type.id != ITEM_EMPTY) {
        HideCursor();
        float scale = 3;
        DrawTextureEx(client->texture_manager.textures[TEXTURE_CURSOR], (Vector2){.x = mousePos.x, .y = mousePos.y}, 0,
                      scale, WHITE);
      } else {
        ShowCursor();
      }

      if (!client_cur_menu_hides_game(client)) {
        client_render_overlay(client);
      }
    }

    // Always render menus

    client_render_menu(client);
  }

  EndDrawing();
}