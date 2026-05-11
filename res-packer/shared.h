#pragma once

#if !defined(__STDC_VERSION__)
#define constexpr const
#elif __STDC_VERSION__ < 202311L
#define constexpr const
#endif

typedef struct {
  // PARTS
  // Directory the file is in
  const char *dir;
  // Name of the file without file extension
  const char *name;
  // File extension without the dot
  const char *file_ext;

  const char *full_path;
} FileEntry;
