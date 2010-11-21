#ifndef LOOP_UNROLLING_HPP_
#define LOOP_UNROLLING_HPP_

#include <iostream>

template <int i>
struct Loop
{
  static inline void exec()
  {
    Loop<i - 1>::exec();
  }
};

template<>
struct Loop<0>
{
  static inline void exec()
  {
  }
};

#endif
