project('chess', 'c', default_options: ['c_std=c99', 'warning_level=3'])
cc = meson.get_compiler('c')
math_dep = cc.find_library('m')

inc = include_directories('include')
inc_utils = include_directories('include/utils')

subdir('src')
