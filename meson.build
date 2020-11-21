project(
    'jormungandr',
    'cpp',
    version: '0.0.42-B',
    default_options: [
        'cpp_std=c++2a',
        'buildtype=debugoptimized',
    ]
)

sources = [
    'src/main.cpp',
    'src/decode/bitreader.cpp',
    'src/decode/property.cpp',
    'src/decode/webgraph.cpp',
    'src/encode/bitwriter.cpp',
    'src/encode/webgraph.cpp',
    'src/graph/propertymap.cpp',
    'src/utility.cpp'
]

executable(
    'jormungandr',
    sources,
    install: true,
    build_by_default: true,
    include_directories: [include_directories('src'), include_directories('include')],
)
