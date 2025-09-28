# warning_level=2 == -Wall -Wextra
# we cant use -Wpedantic because we use gnu statement expressions
project(
  'chess',
  'c',
  default_options: ['c_std=gnu11', 'warning_level=2', 'optimization=0'],
)
cc = meson.get_compiler('c')

# setting a specific standard adds -Wpedantic implicitly, so we disable it here
add_project_arguments(
  cc.get_supported_arguments(['-Wno-pedantic']),
  language: 'c',
)
math_dep = cc.find_library('m')
threads_dep = dependency('threads')

subdir('src')
subdir('tests')
