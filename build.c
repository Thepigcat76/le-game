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

static void visit_entry(struct file_entry entry) {
  if (strcmp(entry.file_ext, "c") == 0)
    cmd_appendf(&cmd, "%s", entry.path);
}

int main(int argc, char **argv) {
  // The compiler to use
  cmd_appendf(&cmd, COMPILER);
  // Flags
  cmd_appendf(&cmd, "-g");
  cmd_appendf(&cmd, "-rdynamic");
  cmd_appendf(&cmd, "-std=%s", STANDARD);
  // Define Flags
  cmd_appendf(&cmd, "-DTARGET=" TARGET_LINUX);
  cmd_appendf(&cmd, "-DCOZY_WRATH_VERSION=" COZY_WRATH_VERSION);
  cmd_appendf(&cmd, "-DCOZY_WRATH_VERSION_RELEASE_DATE=" COZY_WRATH_VERSION_RELEASE_DATE);
  // Output location
  cmd_appendf(&cmd, "-o");
  cmd_appendf(&cmd, OUT_NAME);

  // Adding src files
  walk_dir("src", visit_entry);

  // Libraries
  cmd_appendf(&cmd, "-l%s", LIB_LILC);
  cmd_appendf(&cmd, "-l%s", LIB_RAYLIB);
  cmd_appendf(&cmd, "-l%s", LIB_GL);
  cmd_appendf(&cmd, "-l%s", LIB_MATH);
  cmd_appendf(&cmd, "-l%s", LIB_DL);
  cmd_appendf(&cmd, "-l%s", LIB_RT);
  cmd_appendf(&cmd, "-l%s", LIB_PTHREAD);
  cmd_appendf(&cmd, "-l%s", LIB_CJSON);

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
