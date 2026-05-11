// To build the project, compile this file with a compiler of your choice and
// run the compiled executable. The project also requires the gurd header, which
// can be found at <https://github.com/Thepigcat76/gurd/blob/main/gurd.h>

#include "gurd.h"

#define COMPILER "clang"
#define STANDARD "gnu23"
#define DEBUG true
#define OUT_NAME "build/cozy-wrath"

#define LIB_LILC "lilc"
#define LIB_RAYLIB "raylib"
#define LIB_GL "GL"
#define LIB_MATH "m"
#define LIB_DL "dl"
#define LIB_RT "rt"
#define LIB_PTHREAD "pthread"
#define LIB_CJSON "cjson"

#define COZY_WRATH_VERSION "\\\"0.1\\\""
#define COZY_WRATH_VERSION_RELEASE_DATE "\\\"2026-04-26\\\""

#define TARGET_DEBUG_BUILD "DEBUG_BUILD"
#define TARGET_LINUX "TARGET_LINUX"
#define TARGET_WINDOWS "TARGET_WIN"

static Cmd cmd = {0};
static bool compile_error = false;

static Cmd pack_cmd = {0};

static void visit_entry(struct file_entry entry) {
  if (entry.file_ext == NULL || strcmp(entry.file_ext, "c") != 0)
    return;

  Cmd compile_cmd = {0};

  cmd_appendf(&compile_cmd, "ccache");
  cmd_appendf(&compile_cmd, COMPILER);

  cmd_appendf(&compile_cmd, "-c");
  cmd_appendf(&compile_cmd, "%s", entry.path);

  cmd_appendf(&compile_cmd, "-o");

  const char *src_path = strncmp(entry.path, "src/", 3) == 0 ? entry.path + 4 : entry.path;

  cmd_appendf(&compile_cmd, "./build/%s.o", src_path);
  // Flags
  cmd_appendf(&compile_cmd, "-g");
  cmd_appendf(&compile_cmd, "-std=%s", STANDARD);
  // Define Flags
  cmd_appendf(&compile_cmd, "-DTARGET=" TARGET_LINUX);
  cmd_appendf(&compile_cmd, "-DCOZY_WRATH_VERSION=" COZY_WRATH_VERSION);
  cmd_appendf(&compile_cmd, "-DCOZY_WRATH_VERSION_RELEASE_DATE=" COZY_WRATH_VERSION_RELEASE_DATE);

  char build_path[512];
  sprintf(build_path, "./build/%s.o", src_path);
  ensure_parent_dirs(build_path, 0755);

  printf("Compiling: %s\n", entry.path);

  if (cmd_execute(&compile_cmd) != 0) {
    compile_error = true;
  }

}

static void visit_obj_entry(struct file_entry entry) {
  if (entry.file_ext == NULL || strcmp(entry.file_ext, "o") != 0)
    return;

  cmd_appendf(&cmd, "%s", entry.path);
}

static void visit_res_packer_src_entry(struct file_entry entry) {
  if (entry.file_ext == NULL || strcmp(entry.file_ext, "c") != 0)
    return;

  cmd_appendf(&pack_cmd, "%s", entry.path);
}

static int pack_resources(void) {
  cmd_appendf(&pack_cmd, COMPILER);
  cmd_appendf(&pack_cmd, "-std=" STANDARD);

  walk_dir("res-packer", visit_res_packer_src_entry);

  cmd_appendf(&pack_cmd, "-o build/res-packer");

  cmd_appendf(&pack_cmd, "-l%s", LIB_LILC);

  cmd_execute(&pack_cmd);

  return systemf("./build/res-packer res");
}

int main(int argc, char **argv) {
  remove_dir_recursive("build", false);

  if (argc >= 3 && strcmp(argv[2], "--pack-res") == 0) {
    pack_resources();
  }

  visit_entry((struct file_entry){
    .name = "out.c",
    .file_ext = "c",
    .path = "packed-res/out.c"
  });

  // Adding src files
  walk_dir("src", visit_entry);

  if (compile_error)
    return 1;

  cmd_appendf(&cmd, COMPILER);

  walk_dir("build", visit_obj_entry);

  // Libraries
  cmd_appendf(&cmd, "-l%s", LIB_LILC);
  cmd_appendf(&cmd, "-l%s", LIB_RAYLIB);
  cmd_appendf(&cmd, "-l%s", LIB_GL);
  cmd_appendf(&cmd, "-l%s", LIB_MATH);
  cmd_appendf(&cmd, "-l%s", LIB_DL);
  cmd_appendf(&cmd, "-l%s", LIB_RT);
  cmd_appendf(&cmd, "-l%s", LIB_PTHREAD);
  cmd_appendf(&cmd, "-l%s", LIB_CJSON);

  cmd_appendf(&cmd, "-o");
  cmd_appendf(&cmd, OUT_NAME);

  cmd_appendf(&cmd, "-rdynamic");

  cmd_fprint(&cmd, stdout);
  putchar('\n');
  fflush(stdout);

  // Run the command
  cmd_execute(&cmd);

  if (argc > 1) {
    if (strcmp(argv[1], "r") == 0) {
      if (argc > 2) {
        char args[1024];
        for (int i = 2; i < argc; i++) {
          strcat(args, argv[i]);
          if (i - 1 == argc) {
            strcat(args, " ");
          }
        }
        systemf("./%s %s", OUT_NAME, args);
      } else {
        systemf("./%s", OUT_NAME);
      }
    } else if (strcmp(argv[1], "server") == 0) {
    }
  }
}
