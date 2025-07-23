#include "gurd.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t _interal_cache_src_files_amount = 0;

static char _internal_cache_src_files[256][256] = {};

static void collect_src_files_for_cache(char *directory) {
  size_t src_files_amount = 0;

  struct dirent *entry;
  DIR *dp = opendir(directory);
  if (dp == NULL) {
    perror("opendir");
    exit(1);
  }

  while ((entry = readdir(dp)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    if (entry->d_type == DT_DIR) {
      char dir_buf[256];
      strcpy(dir_buf, str_fmt("%s/%s/", directory, entry->d_name));
      collect_src_files_for_cache(dir_buf);
      continue;
    }

    const char *ext;
    const char *dot = strrchr(entry->d_name, '.');
    if (dot != NULL) {
      ext = dot + 1;
      if (ext && strcmp(ext, "c") == 0) {
        strcpy(_internal_cache_src_files[_interal_cache_src_files_amount], str_fmt("%s/%s", directory, entry->d_name));
        _interal_cache_src_files_amount++;
      }
    }
  }

  closedir(dp);
}

static void cached_compile(const char *compiler, const char *libs, const char *flags, const char *defined_flags, const char *extra_flags,
                           const char *out_dir, const char *out_name) {
  printf("Def flags: %s\n", defined_flags);
  char out_files[256][256] = {};
  size_t out_files_amount = 0;

  for (int i = 0; i < _interal_cache_src_files_amount; i++) {
    char dir_buf[256];
    char file_buf[256];

    strcpy(dir_buf, _internal_cache_src_files[i]);
    strcpy(file_buf, _internal_cache_src_files[i]);

    char *slash = strrchr(dir_buf, '/');
    if (slash != NULL) {
      *slash = '\0';
      make_dir(dir_buf);
    }

    char *filename = strrchr(file_buf, '/');
    filename = filename ? filename + 1 : file_buf;
    filename[strlen(filename) - 1] = 'o';

    const char *extra_compile_flags = ""; //"-Wall -Wextra";

    compile("ccache %s -c %s -o ./build/%s %s %s %s", compiler, _internal_cache_src_files[i], filename, flags, defined_flags,
            extra_compile_flags);
    sprintf(out_files[out_files_amount++], "./build/%s", filename);
  }

  char link_cmd[8192] = "";

  strcat(link_cmd, compiler);
  strcat(link_cmd, " ");

  for (int i = 0; i < out_files_amount; i++) {
    strcat(link_cmd, out_files[i]);
    strcat(link_cmd, " ");
  }

  strcat(link_cmd, libs);
  strcat(link_cmd, " ");

  strcat(link_cmd, "-o ");
  strcat(link_cmd, out_dir);
  strcat(link_cmd, "/");
  strcat(link_cmd, out_name);

  compile("%s %s", link_cmd, extra_flags);
}

static char _internal_def_flags[512] = "";
static char _internal_ex_flags[512] = "";

static char *build_def_flags(char **def_flags) {
  for (int i = 0; def_flags[i] != NULL; i++) {
    strcat(_internal_def_flags, str_fmt("-D%s ", def_flags[i]));
  }
  return _internal_def_flags;
}

static char *build_ex_flags(char **ex_flags) {
  for (int i = 0; ex_flags[i] != NULL; i++) {
    strcat(_internal_ex_flags, ex_flags[i]);
    strcat(_internal_ex_flags, " ");
  }
  return _internal_ex_flags;
}

static char *link_win_libs(char **libraries) {
  _internal_libs_buf[0] = '\0';
  if (libraries == NULL) {
    return _internal_libs_buf;
  }

  for (int i = 0; libraries[i] != NULL; i++) {
    strcat(_internal_libs_buf, str_fmt("-l%s ", libraries[i]));
  }
  printf("%s\n", _internal_libs_buf);
  return _internal_libs_buf;
}

static char *link_libs_target(char **libraries, int target) {
  // WINDOWS
  if (target == 1) {
    strcat(_internal_libs_buf, "-L/usr/x86_64-w64-mingw32/lib/ ");
  }

  return link_libs(libraries);
}
