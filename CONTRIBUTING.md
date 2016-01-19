# Contributing

Any pull requests are welcomed. If you have any particular issues and/or
questions feel free to make these also.

If your pull requests follow the following style that is much appreciated.
If you have minor details or questions before writing, don't fret.

## Code Style

You can use the projects clang-format file to roughly align your code style
with this project.

The generic main points of this are:
    - [LLVM Style](http://llvm.org/docs/CodingStandards.html)
    - 4 spaces (no tabs)

## C++17

C++17 features for the moment are used as needed. If further support for older
compiler is considered a priority at some stage, these can be removed and C++14
can be targetted.

Features used currently are:
 - std::experimental::optional
 - std::experimental::dynarray
 - nested namespaces
