project(
    'jormungandr',
    'cpp',
    version: '0.0.0',
    default_options: [
        'cpp_std=c++2a',
        'buildtype=debugoptimized',
    ]
)

sources = [
    'src/main.cpp',
]

executable(
    'jormungandr',
    sources,
    install: true,
    build_by_default: true,
    include_directories: include_directories('src'),
)
