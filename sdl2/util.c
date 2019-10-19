#include "util.h"

#include <stdlib.h>

void shuffle(int *list, int len)
{
  for (int i = len - 1; i >= 0; --i)
  {
    int r = rand() % len;
    int temp = list[i];
    list[i] = list[r];
    list[r] = temp;
  }
}
