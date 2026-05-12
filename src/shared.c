#include "../include/shared.h"
#include <lilc/alloc.h>
#include <lilc/str.h>
#ifndef TARGET_WIN
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#endif
#include "dirent.h"
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#if defined (TARGET_LINUX) && defined (DEBUG_BUILD)
#include <execinfo.h>
#endif

void shared_setup() {
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

dyn_string_t read_file_to_string(const char *filename, Allocator *allocator) {
  dyn_string_t str = {0};
  
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Error opening file %s - ", filename);
    perror("");
    return str;
  }

  // Seek to the end of the file to get the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET); // Go back to the beginning of the file

  if (file_size < 0) {
    perror("Error determining file size");
    fclose(file);
    return str;
  }

  // Allocate memory for the file content + null terminator
  char *buffer = (char *)allocator->alloc(allocator, file_size + 1);
  if (buffer == NULL) {
    perror("Error allocating memory");
    fclose(file);
    return str;
  }

  // Read the file content into the buffer
  size_t read_size = fread(buffer, 1, file_size, file);
  if (read_size != file_size) {
    perror("Error reading file");
    allocator->dealloc(allocator, buffer);
    fclose(file);
    return str;
  }

  // Null-terminate the string
  buffer[file_size] = '\0';

  fclose(file);

  str.allocator = allocator;
  str.capacity = file_size + 1;
  str.len = file_size;
  str.string = buffer;
  str.term_len = file_size + 1;

  return str;
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
#ifndef TARGET_WIN
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
#if defined (TARGET_LINUX) && defined (DEBUG_BUILD)
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
#endif
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
  char *buf = allocator->alloc(allocator, len);
  strncpy(buf, input, len);
  return buf;
}

inline char *btos(bool b) {
  return b ? "true" : "false";
}

inline bool str_eq(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}

i32 ensure_parent_dirs(const char *filepath, mode_t mode) {
  char dir[PATH_MAX];
  if (snprintf(dir, sizeof dir, "%s", filepath) >= (int)sizeof dir) {
    errno = ENAMETOOLONG;
    return -1;
  }

  char *slash = strrchr(dir, '/');
  if (!slash)
    return 0; // no directory component
  if (slash == dir)
    return 0; // parent is "/"

  *slash = '\0'; // keep just the directory part
  if (!*dir) {
    errno = EINVAL;
    return -1;
  }

  char tmp[PATH_MAX];
  if (snprintf(tmp, sizeof tmp, "%s", dir) >= (int)sizeof tmp) {
    errno = ENAMETOOLONG;
    return -1;
  }

  size_t len = strlen(tmp);
  if (len > 1 && tmp[len - 1] == '/')
    tmp[len - 1] = '\0';

  for (char *p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      if (mkdir(tmp, mode) != 0 && errno != EEXIST)
        return -1;
      *p = '/';
    }
  }
  if (mkdir(tmp, mode) != 0 && errno != EEXIST)
    return -1;
  return 0;
}
