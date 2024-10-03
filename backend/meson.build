project('my_project', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++23'])

# Define project-wide include directories
project_inc = include_directories('src')

# Add subdirectories
subdir('src')
subdir('tests')

# Declare the main executable
executable('my_project',
           'src/main.cpp',
           include_directories : project_inc,
           dependencies : [tokenizers_dep, utils_dep],
           install : true)