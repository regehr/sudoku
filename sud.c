/*

------------------------------------------------------------------

Copyright (c) 2006 University of Utah and the Flux Group.
All rights reserved.

Permission to use, copy, modify, distribute, and sell this software
and its documentation is hereby granted without fee, provided that the
above copyright notice and this permission/disclaimer notice is
retained in all copies or modified versions, and that both notices
appear in supporting documentation.  THE COPYRIGHT HOLDERS PROVIDE
THIS SOFTWARE "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE COPYRIGHT
HOLDERS DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER
RESULTING FROM THE USE OF THIS SOFTWARE.

Users are requested, but not required, to send to csl-dist@cs.utah.edu
any improvements that they make and grant redistribution rights to the
University of Utah.

Author: John Regehr (regehr@cs.utah.edu)

------------------------------------------------------------------

Compile like this:

gcc -Wall -g -O3 sud.c -o sud

Input files look like this:

29-----7-
3-6--84--
8---4---2
-2--31--7
----8----
1--95--6-
7---9---1
--12--3-6
-3-----59

4---3----
---6--8--
--------1
----5--9-
-8----6--
-7-2-----
---1-27--
5-3----4-
9--------

------------------------------------------------------------------

*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned char cell;

struct sud {
  cell board[9][9];
};

static void print(struct sud *s) {
  int i, j;
  for (i = 0; i < 9; i++) {
    if (i % 3 == 0)
      printf("-------------\n");
    for (j = 0; j < 9; j++) {
      if (j % 3 == 0)
        printf("|");
      cell c = s->board[i][j];
      if (c == 0) {
        printf(" ");
      } else if (c >= 1 && c <= 9) {
        printf("%d", c);
      } else {
        assert(0);
      }
    }
    printf("|");
    printf("\n");
  }
  printf("-------------\n");
}

static void clear(cell found[10]) {
  int x;
  for (x = 1; x <= 9; x++) {
    found[x] = 0;
  }
}

static int check(cell found[10]) {
  int x;
  for (x = 1; x <= 9; x++) {
    if (found[x] > 1)
      return 0;
  }
  return 1;
}

static int check_row(int i, struct sud *s) {
  cell found[10] = {0};
  for (int j = 0; j < 9; j++) {
    int index = s->board[i][j];
    assert(index >= 0 && index <= 9);
    found[index]++;
    if (index > 0 && found[index] > 1)
      return 0;
  }
  return 1;
}

static int check_col(int i, struct sud *s) {
  int j;
  cell found[10];
  clear(found);
  for (j = 0; j < 9; j++) {
    found[s->board[j][i]]++;
  }
  return check(found);
}

static int check_region(int ii, int jj, struct sud *s) {
  ii *= 3;
  jj *= 3;
  int i, j;
  cell found[10];
  clear(found);
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      found[s->board[ii + i][jj + j]]++;
    }
  }
  return check(found);
}

static int valid(struct sud *s) {
  {
    int i;
    for (i = 0; i < 9; i++) {
      if (!check_row(i, s))
        return 0;
    }
  }
  {
    int j;
    for (j = 0; j < 9; j++) {
      if (!check_col(j, s))
        return 0;
    }
  }
  {
    int i, j;
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        if (!check_region(i, j, s))
          return 0;
      }
    }
  }
  return 1;
}

static int incremental_valid(struct sud *s, int i, int j) {
  if (!check_row(i, s))
    return 0;
  if (!check_col(j, s))
    return 0;
  if (!check_region(i / 3, j / 3, s))
    return 0;
  return 1;
}

static long long int solns;

static void solve(struct sud s, int filled) {
  if (filled == 81) {
    print(&s);
    solns++;
  } else {
    int i, j, k;
    for (i = 0; i < 9; i++) {
      for (j = 0; j < 9; j++) {
        if (s.board[i][j] == 0) {
          for (k = 1; k <= 9; k++) {
            s.board[i][j] = k;
            if (incremental_valid(&s, i, j)) {
              solve(s, filled + 1);
            }
          }
          return;
        }
      }
    }
  }
}

static int load(char *fn, struct sud *s) {
  FILE *inf = fopen(fn, "r");
  assert(inf);
  int i, j, x;
  int entries = 0;
  char str[20];
  int filled = 0;
  for (i = 0; i < 9; i++) {
    int x = fscanf(inf, "%s", str);
    assert(x == 1);
    for (j = 0; j < 9; j++) {
      cell c = str[j];
      cell v;
      if (c == '-') {
        v = 0;
      } else if (c >= '0' && c <= '9') {
        v = c - '0';
        filled++;
      } else {
        printf("error in input file\n");
        exit(-1);
      }
      s->board[i][j] = v;
      entries++;
    }
  }
  x = fscanf(inf, "%s", str);
  assert(x == -1);

  assert(entries == 81);

  printf("original configuration:\n");
  print(s);
  printf("%d entries filled\n", filled);

  return filled;
}

/*
static unsigned long long rdtsc (void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
*/

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: ./sud input_file.txt\n");
    return -1;
  }

  struct sud s;
  int filled = load(argv[1], &s);

  if (valid(&s)) {
    solns = 0;
    printf("solution:\n");
    // unsigned long long before, after;
    // before = rdtsc();
    solve(s, filled);
    // after = rdtsc();
    // printf ("found %lld solutions in %llu cycles\n", solns, after-before);
    printf("found %lld solutions\n", solns);
  } else {
    printf("original puzzle isn't valid!\n");
  }

  return 0;
}
