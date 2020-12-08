project(
    'jormungandr',
    'cpp',
    version: '0.0.42-B',
    default_options: [
        'cpp_std=c++2a',
        'buildtype=debugoptimized',
        'default_library=static'
    ]
)

sources = [
    'src/decode/bitreader.cpp',
    'src/decode/property.cpp',
    'src/encode/bitwriter.cpp',
    'src/encode/property.cpp',
    'src/graph/propertymap.cpp',
    'src/utility.cpp',
    'src/encoding.cpp',
]

include = include_directories('include')

lib = library(
    'jormungandr',
    sources,
    include_directories: include
)

jormungandr_dep = declare_dependency(
    include_directories: include,
    link_with: lib
)

executable(
    'jormungandr',
    'src/main.cpp',
    install: true,
    build_by_default: true,
    include_directories: include,
    link_with: lib
)

executable(
    'recode',
    'src/recode.cpp',
    install: true,
    build_by_default: true,
    include_directories: include,
    link_with: lib
)

executable(
    'jormungandr-benchmark',
    'src/benchmark.cpp',
    install: true,
    build_by_default: true,
    include_directories: include,
    link_with: lib
)
