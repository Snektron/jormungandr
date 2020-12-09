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

Jormungandr may also be used from other Meson projects by means of a subproject:

```
dep = subproject('jormungandr').get_variable('jormungandr_dep')
```

## Benchmarking

The default target generates the `jormungandr-benchmark` executable. This simply measures the time to encode or decode a webgraph file in nanoseconds. See src/benchmark.cpp for further details. Benchmarks can be performed automatically by downloading webgraph files (from https://law.di.unimi.it/ for example) into the test/ directory, and executing the `ninja bench-jormungandr`. This calls `benchmark.sh`, which then gathers the results. See `benchmark.sh` for further details.

This repository also contains the means to compare to the original WebGraph implementation. `benchmark/` contains a small java program, which usage is similar to that of jormunhandr-benchmark. Executing `ninja bench-webgraph` performs the same exact benchmarks as the `bench-jormungandr` target.
