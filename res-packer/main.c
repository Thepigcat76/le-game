#include "cli.h"

int main(i32 argc, char **argv) {
  CliArgs args = {0};

  args_parse(&args, argc, argv);

  args_handle(&args);
}
