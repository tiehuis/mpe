# mptet

![mptet-Terminal](http://i.imgur.com/MU4zaw0.png)

A tetris game implemented in modern C++. The aim of this project is to
implement a relatively clean version of tetris with fairly expansive
support for a number of different rulesets and configurations.

This aims to be relatively feature complete in terms of the base engine and
provide a simple enough mechanism to utilize this engine for a wide-ranging
reach of different frontends. Dependencies should be limited, and there should
exist some version that can run with no external dependencies.

## Installation

```
git clone https://gitlab.com/tiehuis/mptet
cd mptet
./waf configure
./waf build
```

mptet uses a few experimental C++17 features, so a modern compiler (clang, gcc)
is likely required to compile the code. These may be replaced with C++14
features if needed.

## Focus

The focus of this is to provide a robust tetris implementation which can be
easily extended for any purpose.

The following features are considered to be essential:

 - A number of *Accurate* rotation systems.
 - Accurate timing for high precision play.
 - Small memory footprint.
 - Small easily hackable codebase.
 - Cross-Platform

## Current Objectives

### Build System

This project uses the [waf](https://github.com/waf-project/waf) build system.
There is a strong requirement to allow for a great degree of freedom when
building given the possible future options, and the wscript needs a lot of work
in that regard.

### Graphical Frontends

It would be nice to have a number of graphical frontends. This provides users
with a lot of freedom about which programs they are required to have and we can
provide the best frontend for their system.

The current main focus implementations are the Terminal and SDL2 versions.

### Web Support

There are some interesting projects which aim to compile native code to a
format which allows a browser-based backend. Of these [Emscripten](http://kripken.github.io/emscripten-site/)
and Google's [Native Client](https://developer.chrome.com/native-client) are
of keen interest to have implementations for.

### Testing System

There exists a need to allow for automatic testing of components of the system.
These should come in the form of unit tests, which can then be combined with
a continuous integration system to ensure that regressions do not occur.

### Multiple Resolutions

A smart way of dealing with a range of resolutions should be researched and
implemented. Current games often are fixed to a particular fixed size and don't
make full use of current widescreen technology.

### Game Recordings

It would be valuable to allow recordings of games to be made.
