#include "shuffle.h"

#include <stdlib.h>

void shuffle(std::vector<int> &list)
{
    for (int i = list.size(); i >= 0; --i)
    {
        int r = rand() % list.size();
        int temp = list[i];
        list[i] = list[r];
        list[r] = temp;
  }
}