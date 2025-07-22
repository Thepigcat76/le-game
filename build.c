/* To build the project, compile this file with a compiler of your choice and run the compiled executable.
 * The project also requires the gurd header, which can be found at <https://github.com/Thepigcat76/gurd/blob/main/gurd.h>
 */

#include "gurd.h"
#include <string.h>

typedef enum {
  TARGET_LINUX,
  TARGET_WIN,
} BuildTarget;

typedef struct {
  char *compiler;
  bool debug;
  bool release;
  char *std;
  BuildTarget target;
  char *out_dir;
  char *out_name;
  char *libraries[64];
  char *define_flags[64];
  char *extra_flags[64];
} BuildOptions;

static BuildOptions OPTS = {.compiler = "clang",
                            .debug = true,
                            .release = false,
                            .std = "gnu23",
                            .target = TARGET_LINUX,
                            .out_dir = "./build/",
                            .out_name = "cozy-wrath",
                            .libraries = ARRAY("raylib", "GL", "m", "pthread", "dl", "rt", "X11", "cjson"),
                            .define_flags = ARRAY("DEBUG_BUILD"),
                            .extra_flags = ARRAY("-rdynamic")};

static size_t _interal_cache_src_files_amount = 0;

static char _internal_cache_src_files[256][256] = {};

static void cached_compile(const char *compiler, const char *libs, const char *flags, const char *defined_flags, const char *extra_flags,
                           const char *out_dir, const char *out_name);

static void collect_src_files_for_cache(char *directory);

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
  }
  return _internal_ex_flags;
}

int main(int argc, char **argv) {
  char *compiler = build_compiler(OPTS.compiler, OPTS.target);
  collect_src_files_for_cache("./src/");
  char *libraries = link_libs(OPTS.libraries);
  char *flags = build_flags(&OPTS);
  char *out_name = build_name(OPTS.out_name, OPTS.target);
  char *defined_flags = build_def_flags(OPTS.define_flags);
  char *extra_flags = build_ex_flags(OPTS.extra_flags);

  make_dir(OPTS.out_dir);
  cached_compile(OPTS.compiler, libraries, flags, defined_flags, extra_flags, OPTS.out_dir, out_name);
  printf("Cmd: %s\n", _internal_cmd_buf);

  if (argc >= 2) {
    if (STR_CMP_OR(argv[1], "r", "run")) {
      run(OPTS.out_dir, out_name, argc, argv);
      return 0;
    } else if (STR_CMP_OR(argv[1], "d", "dbg")) {
      dbg(OPTS.out_dir, out_name, OPTS.debug);
      return 0;
    } else {
      fprintf(stderr, "[Error]: Invalid first arg: %s\n", argv[1]);
      return 1;
    }
  }
}

static char *build_name(char *default_name, int target) {
  if (target == TARGET_WIN) {
    strcpy(_internal_build_name_buf, str_fmt("%s.exe", default_name));
    return _internal_build_name_buf;
  }
  return default_name;
}

static char *build_compiler(char *default_compiler, int target) {
  if (target == TARGET_WIN) {
    return "x86_64-w64-mingw32-gcc";
  }
  return default_compiler;
}

static char *build_flags(void *_opts) {
  BuildOptions *opts = (BuildOptions *)_opts;
  _internal_flags_buf[0] = '\0';

  if (opts->debug) {
    strcat(_internal_flags_buf, "-g ");
  } else if (opts->release) {
    strcat(_internal_flags_buf, "-O3 ");
  }

  if (!opts->release) {
    strcat(_internal_flags_buf, "-O0 ");
  }

  if (!opts->debug) {
    strcat(_internal_flags_buf, "-g0 ");
  }

  if (opts->std != NULL) {
    strcat(_internal_flags_buf, str_fmt("-std=%s ", opts->std));
  }

  return _internal_flags_buf;
}

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

    compile("ccache %s -c %s -o ./build/%s %s %s", compiler, _internal_cache_src_files[i], filename, flags, defined_flags);
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
