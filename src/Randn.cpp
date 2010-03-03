#include "Randn.h"

int	rand_n(int n)
{
  int partSize   = 1 + (n == RAND_MAX ? 0 : (RAND_MAX - n) / (n + 1));
  int maxUsefull = partSize * n + (partSize - 1);
  int draw;

  do
    {
      draw = rand();
    } while (draw > maxUsefull);
  return draw / partSize;
}
