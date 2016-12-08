#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "./sand.gen.h"

#define COLOR true
const uint8_t MAX_HEIGHT = 4;

// just parse the command line args.
uint64_t parse_arg(char *str, bool odd, char *err_str) {
  uint64_t parsed = strtoull(str, NULL, 10);
  if (parsed == 0 || (odd && parsed % 2 == 0)) {
    fprintf(stderr, "[ERROR] invalid %s: '%s'\n", err_str, str);
    exit(1);
  }
  return parsed;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "[ERROR] usage: %s <rows> <columns> <center-height>\n",
            argv[0]);
    return 1;
  }
  uint64_t rows = parse_arg(argv[1], true, "number of rows");
  uint64_t cols = parse_arg(argv[2], true, "number of columns");
  uint64_t center = parse_arg(argv[3], false, "center height");
  pile *pile = new_pile(rows, cols, center);

  print_pile(evolve(pile, MAX_HEIGHT, true, COLOR), COLOR);

  return 0;
}