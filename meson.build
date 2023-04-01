project('learning_gl', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])
deps = []
deps += dependency('glfw3',
  default_options: ['install=true', 'display-api=win32', 'default_library=static']
)
deps += dependency('glad')

exe = executable('learning_gl', 'learning_gl.cpp',
  dependencies: deps,
  install : true)

