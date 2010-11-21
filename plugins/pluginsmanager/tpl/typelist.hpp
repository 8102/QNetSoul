// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.

#ifndef TYPE_LIST_HPP_
#define TYPE_LIST_HPP_

class IPopupPlugin;
class IWidgetPlugin;

template <class T, class U>
struct Typelist
{
  typedef T Head;
  typedef U Tail;
};

class NullType {};

// PluginInterfaces is a list of types
typedef Typelist<IPopupPlugin*,
		 Typelist<IWidgetPlugin*,
			  NullType>
		 > PluginInterfaces;


template <class TList, unsigned int index> struct TypeAt;

template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0>
{
  typedef Head Result;
};
template <class Head, class Tail, unsigned int i>
struct TypeAt<Typelist<Head, Tail>, i>
{
  typedef typename TypeAt<Tail, i - 1>::Result Result;
};

#endif
