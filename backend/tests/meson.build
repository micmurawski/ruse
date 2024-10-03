gtest_dep = dependency('gtest')

test('analysis_test',
     executable('test_analysis',
                'test_analysis.cpp',
                include_directories : project_inc,
                dependencies : [gtest_dep, tokenizers_dep]))


test('filters_test',
     executable('test_filters',
                'test_filters.cpp',
                include_directories : project_inc,
                dependencies : [gtest_dep, filters_dep]))

test('utils_test',
     executable('test_utils',
                'test_utils.cpp',
                include_directories : project_inc,
                dependencies : [gtest_dep, utils_dep]))