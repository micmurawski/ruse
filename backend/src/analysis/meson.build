core_lib = static_library('core', 'core.cpp')

tokenizers_lib = static_library(
    'tokenizers',
    'tokenizers.cpp',
    link_with: core_lib,
    include_directories : ['.', '..']
)

tokenizers_dep = declare_dependency(
    link_with : tokenizers_lib,
    include_directories : ['.', '..']
)

