project(
	'chess',
	'c',
	default_options: ['c_std=c99', 'warning_level=3', 'optimization=2'],
)
cc = meson.get_compiler('c')
math_dep = cc.find_library('m')

subdir('src')
subdir('tests')
