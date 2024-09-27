gtest_dep = dependency('gtest')

test('analysis_test',
     executable('test_analysis',
                'test_analysis.cpp',
                include_directories : project_inc,
                dependencies : [gtest_dep, analysis_dep]))

test('utils_test',
     executable('test_utils',
                'test_utils.cpp',
                include_directories : project_inc,
                dependencies : [gtest_dep, utils_dep]))