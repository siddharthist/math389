#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./sand-lib.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

/* static uint8_t __attribute__((const)) char_to_int(char c) { */
/*   return (uint8_t)c - '0'; */
/* } */
static char __attribute__((const)) int_to_char(uint8_t i) {
  return (char)i + '0';
}

static char *maybe_colorize(char *color, char *str, bool really) {
  if (really) {
    char *new_str = malloc((10 + strlen(str)) * sizeof(char));
    sprintf(new_str, "%s%s", color, str);
    return new_str;
  }
  return str;
}

char *pile_to_string(pile_ *pile, bool color) {
  assert(pile != NULL);
  assert(pile->grid != NULL);
  char *str = malloc((10 * pile->rows * pile->cols) * sizeof(char));
  assert(str != NULL); // make sure we've got memory
  str[0] = '\000';     // zero it

  for (uint16_t i = 0; i < pile->rows; i++) {
    for (uint16_t j = 0; j < pile->cols; j++) {
      switch (pile->grid[i][j]) {
      case 0:
        strcat(str, maybe_colorize(RESET, "0", color));
        break;
      case 1:
        strcat(str, maybe_colorize(BLUE, "1", color));
        break;
      case 2:
        strcat(str, maybe_colorize(GREEN, "2", color));
        break;
      case 3:
        strcat(str, maybe_colorize(YELLOW, "3", color));
        break;
      }
      if (pile->grid[i][j] > 3) {
        if (pile->grid[i][j] < 10) {
          char *char_str = malloc(2 * sizeof(char));
          // we know it's a uint8_t because it's < 10
          char_str[0] = int_to_char((uint8_t)pile->grid[i][j]);
          char_str[1] = '\000';
          strcat(str, maybe_colorize(RED, char_str, color));
          free(char_str);
        } else
          strcat(str, maybe_colorize(RED, "?", color));
      }
    }
    strcat(str, "\n");
  }

  return str;
}

void print_pile(pile_ *pile, bool color) {
  printf("\033[2J\033[1;1H"); // clear the screen
  char *str = pile_to_string(pile, color);
  printf("%s", str);
  free(str);
}

pile_ *new_pile(uint16_t rows, uint16_t cols, uint16_t center, uint16_t max) {
  assert(rows > 0);
  assert(cols > 0);
  assert(max % 4 == 0);

  pile_ *pile = malloc(sizeof(pile_));
  assert(pile != NULL);

  pile->rows = rows;
  pile->cols = cols;
  pile->max = max;

  // allocate the grid
  pile->grid = malloc(rows * sizeof(uint16_t *));
  for (uint16_t i = 0; i < rows; i++) {
    pile->grid[i] = malloc(cols * sizeof(uint16_t));
    for (uint16_t j = 0; j < cols; j++) // zero it out
      pile->grid[i][j] = 0;
  }

  uint16_t center_row = rows / 2;
  uint16_t center_col = cols / 2;
  pile->grid[center_row][center_col] = center;

  return pile;
}

// this is the part based on Jim's pseudocode
// returns whether or not this square is now settled
// preconditions are verified in step()
bool __attribute__((hot))
step_square(pile_ *src, pile_ *dst, uint16_t row, uint16_t col) {
  if (src->grid[row][col] >= src->max)
    dst->grid[row][col] = src->grid[row][col] - src->max;
  else
    dst->grid[row][col] = src->grid[row][col];

  if (row > 0 && src->grid[row - 1][col] >= src->max) // north
    dst->grid[row][col] = dst->grid[row][col] + (src->max / 4);
  if (col > 0 && src->grid[row][col - 1] >= src->max) // east
    dst->grid[row][col] = dst->grid[row][col] + (src->max / 4);
  if (row < src->rows - 1 && src->grid[row + 1][col] >= src->max) // south
    dst->grid[row][col] = dst->grid[row][col] + (src->max / 4);
  if (col < src->cols - 1 && src->grid[row][col + 1] >= src->max) // west
    dst->grid[row][col] = dst->grid[row][col] + (src->max / 4);

  // if we're still over max height in any cell, we're not done
  if (dst->grid[row][col] >= src->max)
    return false;
  return true;
}

// returns whether or not this region is now settled
bool __attribute__((nonnull))
step_region(pile_ *src, pile_ *dst, uint16_t row_beg, uint16_t row_end,
            uint16_t col_beg, uint16_t col_end) {
  assert(src != NULL);
  assert(src->grid != NULL);
  assert(dst != NULL);
  assert(dst->grid != NULL);
  assert(row_end <= src->rows);
  assert(col_end <= src->cols);

  bool done = true;
  for (uint16_t i = row_beg; i < row_end; i++)
    for (uint16_t j = col_beg; j < col_end; j++)
      if (!step_square(src, dst, i, j))
        done = false;

  assert(src != NULL);
  assert(src->grid != NULL);
  assert(dst != NULL);
  assert(dst->grid != NULL);
  return done;
}
