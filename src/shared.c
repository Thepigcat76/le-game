#include "../include/shared.h"
#ifndef SURTUR_BUILD_WIN
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#endif
#include "dirent.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <execinfo.h>

Texture2D TEXT_INPUT_TEXTURE;

Texture2D DEBUG_BUTTON_TEXTURE;
Texture2D DEBUG_BUTTON_SELECTED_TEXTURE;

Texture2D BUTTON_TEXTURE;
Texture2D BUTTON_SELECTED_TEXTURE;

Texture2D BACKPACK_BACK_GROUND;

Texture2D MAIN_HAND_SLOT_TEXTURE;
Texture2D OFF_HAND_SLOT_TEXTURE;

Texture2D SAVE_SLOT_TEXTURE;
Texture2D SAVE_SLOT_SELECTED_TEXTURE;

Texture2D NPC_TEXTURES[DIRECTIONS_AMOUNT];
Texture2D NPC_ANIMATED_TEXTURES[DIRECTIONS_AMOUNT];

Texture NPC_TEXTURE_OLD_MAN;

void shared_init() {
  TEXT_INPUT_TEXTURE = LoadTexture("res/assets/gui/text_input.png");

  DEBUG_BUTTON_TEXTURE = LoadTexture("res/assets/gui/debug_button.png");
  DEBUG_BUTTON_SELECTED_TEXTURE = LoadTexture("res/assets/gui/debug_button_selected.png");

  BUTTON_TEXTURE = LoadTexture("res/assets/gui/button.png");
  BUTTON_SELECTED_TEXTURE = LoadTexture("res/assets/gui/button_selected.png");

  BACKPACK_BACK_GROUND = LoadTexture("res/assets/gui/backpack_slots.png");

  MAIN_HAND_SLOT_TEXTURE = LoadTexture("res/assets/gui/main_hand_slot.png");
  OFF_HAND_SLOT_TEXTURE = LoadTexture("res/assets/gui/off_hand_slot.png");

  SAVE_SLOT_TEXTURE = LoadTexture("res/assets/gui/save_slot.png");
  SAVE_SLOT_SELECTED_TEXTURE = LoadTexture("res/assets/gui/save_slot_selected.png");

  NPC_TEXTURES[DIRECTION_UP] = LoadTexture("res/assets/beings/npc_back.png");
  NPC_TEXTURES[DIRECTION_DOWN] = LoadTexture("res/assets/beings/npc_front.png");
  NPC_TEXTURES[DIRECTION_LEFT] = LoadTexture("res/assets/beings/npc_left.png");
  NPC_TEXTURES[DIRECTION_RIGHT] = LoadTexture("res/assets/beings/npc_right.png");

  NPC_ANIMATED_TEXTURES[DIRECTION_UP] = LoadTexture("res/assets/beings/npc_back_walking.png");
  NPC_ANIMATED_TEXTURES[DIRECTION_DOWN] = LoadTexture("res/assets/beings/npc_front_walking.png");
  NPC_ANIMATED_TEXTURES[DIRECTION_LEFT] = LoadTexture("res/assets/beings/npc_left_walking.png");
  NPC_ANIMATED_TEXTURES[DIRECTION_RIGHT] = LoadTexture("res/assets/beings/npc_right_walking.png");

  NPC_TEXTURE_OLD_MAN = LoadTexture("res/assets/old_man.png");

  srand(time(NULL));
}

void dir_create(const char *dir_name) {
#ifdef _WIN32
#include <direct.h> // For _mkdir on Windows
  _mkdir(dir_name);
#else
#include <sys/stat.h>
#include <sys/types.h>
  mkdir((dir_name), 0777);
#endif
}

bool dir_exists(const char *path) {
  bool result = false;
  DIR *dir = opendir(path);

  if (dir != NULL) {
    result = true;
    closedir(dir);
  }

  return result;
}

bool string_starts_with(const char *str, const char *prefix) { return strncmp(str, prefix, strlen(prefix)) == 0; }

char *read_file_to_string(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Error opening file %s - ", filename);
    perror("");
    return NULL;
  }

  // Seek to the end of the file to get the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET); // Go back to the beginning of the file

  if (file_size < 0) {
    perror("Error determining file size");
    fclose(file);
    return NULL;
  }

  // Allocate memory for the file content + null terminator
  char *buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
    perror("Error allocating memory");
    fclose(file);
    return NULL;
  }

  // Read the file content into the buffer
  size_t read_size = fread(buffer, 1, file_size, file);
  if (read_size != file_size) {
    perror("Error reading file");
    free(buffer);
    fclose(file);
    return NULL;
  }

  // Null-terminate the string
  buffer[file_size] = '\0';

  fclose(file);
  return buffer;
}

int string_contains(const char *string, char c) {
  int len = strlen(string);
  int found = 0;
  for (int i = 0; i < len; i++) {
    if (string[i] == c)
      found++;
  }
  return found;
}

inline Vec2i vec2i(int x, int y) { return (Vec2i){.x = x, .y = y}; }

inline Vec2f vec2f(float x, float y) { return (Vec2f){.x = x, .y = y}; }

Vec2i vec2i_sub(Vec2i vec0, Vec2i vec1) { return vec2i(vec0.x - vec1.x, vec0.y - vec1.y); }

Vec2i vec2i_add(Vec2i vec0, Vec2i vec1) { return vec2i(vec0.x + vec1.x, vec0.y + vec1.y); }

inline void rec_draw_outline(Rectangle rec, Color color) { DrawRectangleLinesEx(rec, 1, color); }

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2) { return vec1->x == vec2->x && vec1->y == vec2->y; }

inline Dimensionsf dimf(float width, float height) { return (Dimensionsf){.width = width, .height = height}; }

inline Rectf rectf_from_dimf(float x, float y, Dimensionsf dimf) { return rectf(x, y, dimf.width, dimf.height); }

inline Rectf rectf(float x, float y, float w, float h) { return (Rectf){.x = x, .y = y, .width = w, .height = h}; }

inline Color color_rgb(int r, int g, int b) { return color_rgba(r, g, b, 255); }

inline Color color_rgba(int r, int g, int b, int a) { return (Color){.r = r, .g = g, .b = b, .a = a}; }

void DrawTextureRecEx(Texture2D texture, Rectangle source, Vector2 pos, float rotation, float scale, Color tint) {
  DrawTexturePro(texture, source,
                 (Rectangle){.x = pos.x, .y = pos.y, .width = source.width * scale, .height = source.height * scale},
                 (Vector2){.x = source.width * scale / 2, .y = source.height * scale / 2}, rotation, tint);
}

int floor_div(int a, int b) { return (a >= 0) ? (a / b) : ((a - b + 1) / b); }

int floor_mod(int a, int b) {
  int r = a % b;
  return (r < 0) ? r + b : r;
}

float lerpf(float a, float b, float t) { return (float)(a + (b - a) * t); }

bool vec2f_eq(Vec2f a, Vec2f b) {
  const float EPSILON = 0.01f;
  return fabsf(a.x - b.x) < EPSILON && fabsf(a.y - b.y) < EPSILON;
}

Direction direction_from_delta(int x, int y) {
  if (x < 0) {
    return DIRECTION_LEFT;
  } else if (x > 0) {
    return DIRECTION_RIGHT;
  } else if (y > 0) {
    return DIRECTION_UP;
  } else {
    return DIRECTION_DOWN;
  }
}

int signum(int s) { return s == 0 ? 0 : s / abs(s); }

float string_to_world_seed(const char *str) {
  if (strlen(str) == 0) {
    return (float)rand() / (float)RAND_MAX;
  }

  unsigned int hash = 0;
  while (*str) {
    hash = hash * 101 + (unsigned char)(*str++);
  }
  return (hash % 100000) / 100000.0f;
}
int ip_addr(char *ip_addr_buf) {
#ifndef SURTUR_BUILD_WIN
  struct ifaddrs *ifaddr, *ifa;

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    return -1;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;

    // Check for IPv4 family
    if (ifa->ifa_addr->sa_family == AF_INET) {
      void *addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, addr_ptr, ip_addr_buf, INET_ADDRSTRLEN);
    }
  }

  freeifaddrs(ifaddr);
#endif
  return 0;
}

void stack_trace_print(void) {
  void *buffer[100];
  int nptrs = backtrace(buffer, 100);
  char **symbols = backtrace_symbols(buffer, nptrs);

  if (symbols == NULL) {
    perror("backtrace_symbols");
    exit(EXIT_FAILURE);
  }

  printf("Stack trace (%d frames):\n", nptrs);
  for (int i = 0; i < nptrs; i++) {
    printf("%s\n", symbols[i]);
  }

  free(symbols);
}


Color color_from_str(const char *color_literal) {
    if (color_literal == NULL) return RAYWHITE;

    if (strcmp(color_literal, "red") == 0) return RED;
    if (strcmp(color_literal, "green") == 0) return GREEN;
    if (strcmp(color_literal, "blue") == 0) return BLUE;
    if (strcmp(color_literal, "yellow") == 0) return YELLOW;
    if (strcmp(color_literal, "orange") == 0) return ORANGE;
    if (strcmp(color_literal, "purple") == 0) return PURPLE;
    if (strcmp(color_literal, "maroon") == 0) return MAROON;
    if (strcmp(color_literal, "lime") == 0) return LIME;
    if (strcmp(color_literal, "darkgreen") == 0) return DARKGREEN;
    if (strcmp(color_literal, "skyblue") == 0) return SKYBLUE;
    if (strcmp(color_literal, "darkblue") == 0) return DARKBLUE;
    if (strcmp(color_literal, "white") == 0) return WHITE;
    if (strcmp(color_literal, "black") == 0) return BLACK;
    if (strcmp(color_literal, "gray") == 0 || strcmp(color_literal, "grey") == 0) return GRAY;
    if (strcmp(color_literal, "lightgray") == 0 || strcmp(color_literal, "lightgrey") == 0) return LIGHTGRAY;
    if (strcmp(color_literal, "darkgray") == 0 || strcmp(color_literal, "darkgrey") == 0) return DARKGRAY;
    if (strcmp(color_literal, "pink") == 0) return PINK;
    if (strcmp(color_literal, "beige") == 0) return BEIGE;
    if (strcmp(color_literal, "brown") == 0) return BROWN;
    if (strcmp(color_literal, "gold") == 0) return GOLD;
    if (strcmp(color_literal, "violet") == 0) return VIOLET;

    return RAYWHITE;
}

char *str_cpy_heap(const char *input) {
  return str_cpy(input, &HEAP_ALLOCATOR);
}

char *str_cpy(const char *input, Allocator *allocator) {
  size_t len = strlen(input) + 1;
  char *buf = allocator->alloc(len);
  strncpy(buf, input, len);
  return buf;
}

char *btos(bool b) {
  return b ? "true" : "false";
}
