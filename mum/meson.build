project('main',
  ['cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'werror=true', 'cpp_std=c++20'])

include = include_directories('./include')

lib = library('library', [
    'src/library.cc'
  ], dependencies: [],
  include_directories: include)

executable('use_benchmark', [
    'src/use_benchmark.cc'
  ], dependencies: [],
  link_with: [lib],
  include_directories: include)

executable('gen_benchmark', [
    'src/gen_benchmark.cc'
  ], dependencies: [],
  link_with: [lib],
  include_directories: include)

executable('compare', [
    'src/compare.cc'
  ], dependencies: [],
  link_with: [lib],
  include_directories: include)

executable('main', [
    'src/main.cc'
  ], dependencies: [],
  link_with: [lib],
  include_directories: include)
