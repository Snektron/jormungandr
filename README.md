# Jormungandr

Jormungandr is a C++20 implementation of the WebGraph compression format. It supports files compressed by the BVGraph class of the original implementation, and supports the gamma, delta, unary and zeta encodings, as well a custom encoding. This implementation was written as part of the SNACS course at Leiden University, fall of 2020.

## Compiling

Jormungandr requires a capable C++2a compiler such as clang or gcc, and uses the Meson build system. No other dependencies are required. The project can be compiled to generate three standalone executables (used for benchmarks and general experimentation) and libjormungandr as follows:

```
$ mkdir build
$ cd build
$ meson ..
$ ninja
```

Jormungandr may also be used from