project('course-work', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])


# my_folder = files(meson.current_source_dir()/'static')

# meson.add_install_script('copy_folder',
#     [meson.current_build_dir() / 'static', my_folder],
#     input : my_folder,
#     output : meson.current_build_dir() / 'static'
# )

deps = []
deps += dependency('glfw3',
  default_options: ['install=true', 'display-api=win32', 'default_library=static']
)
deps += dependency('glad')
deps += dependency('glm')

sources = []
sources += './sources/course_work.cpp'
# sources += './sources/shader.cpp'

headers = [include_directories('./sources')]

exe = executable('course-work', sources,
  include_directories: headers,
  dependencies: deps,
  install : true)

install_subdir('static', install_dir: '')

