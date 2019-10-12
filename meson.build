###################################################################################
#                                                                                 #
# NAME: meson.build                                                               #
#                                                                                 #
# AUTHOR: Zhiqiang Ma.                                                            #
# WRITTEN BY: Michael Brockus.                                                    #
#                                                                                 #
# License: Unlicense                                                              #
#                                                                                 #
###################################################################################



project('zlog', 'c',
    license         : 'Unlicense',
    meson_version   : '>=0.50.0',
    default_options : 
    [ 
        'werror=true',
        'optimization=3', 
        'warning_level=3', 
        'b_sanitize=address,undefined', 
        'b_lto=true', 
        'b_lundef=true'
    ])

lang = 'c'
cc = meson.get_compiler(lang)


##
#
# Meson: Add compiler flags
#
##
if cc.get_id() == 'clang'
    add_project_arguments(cc.get_supported_arguments(
            [
            '-Wweak-vtables', '-Wexit-time-destructors',
            '-Wglobal-constructors', '-Wmissing-noreturn' 
            ]
        ), language: lang)
endif

if cc.get_argument_syntax() == 'gcc'
    add_project_arguments(cc.get_supported_arguments(
            [
            '-Wformat', '-Waddress', '-Winit-self', '-Wno-multichar',
            '-Wpointer-arith'       , '-Wwrite-strings'              , 
            '-Wno-parentheses'      , '-Wno-type-limits'             , 
            '-Wformat-security'     , '-Wunreachable-code'           , 
            '-Waggregate-return'    , '-Wformat-nonliteral'          ,
            '-Wmissing-prototypes'  , '-Wold-style-definition'       ,
            '-Wmissing-declarations', '-Wmissing-include-dirs'       , 
            '-Wno-unused-parameter' , '-Wdeclaration-after-statement'
            ]
        ), language: lang)
endif

if cc.get_id() == 'msvc'
    add_project_arguments(cc.get_supported_arguments(
            [
            '/w44265', '/w44061', '/w44062', 
            '/wd4018', '/wd4146', '/wd4244',
            '/wd4305',
            ]
        ), language: lang)
endif


thread_dep = dependency('threads')


zlog_dir = include_directories('.')

zlog_lib = static_library(meson.project_name(), 
    sources: ['zlog.c'],
    dependencies: thread_dep,
    include_directories: zlog_dir)

zlog_dep = declare_dependency(
    link_with: zlog_lib,
    include_directories: zlog_dir)
