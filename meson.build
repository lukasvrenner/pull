project('pull', 'c', default_options : ['warning_level=3', 'c_std=c17',])
add_project_arguments(language : 'c')

sources = ['src/main.c']
turtls = dependency('turtls', version : '>=0.1.0')
executable('pull', sources, install : true, dependencies : turtls)
