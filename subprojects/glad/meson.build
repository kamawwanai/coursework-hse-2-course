project('glad', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_GLAD']

install_headers('include/glad/glad.h', 'include/KHR/khrplatform.h', subdir: 'glad')

shlib = static_library('glad', 'src/glad.c',
  install : true,
  include_directories : ['include'],
  c_args : lib_args,
  gnu_symbol_visibility : 'hidden',
)

# Make this library usable as a Meson subproject.
glad_dep = declare_dependency(
  include_directories: include_directories(['include']),
  link_with : shlib)
