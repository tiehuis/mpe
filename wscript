#!/usr/bin/env python

APPNAME = 'mptet'
VERSION = '0.2'

top = '.'
out = 'build'

'''------------------------------------------
                   Options
   ---------------------------------------'''
def options(ctx):
    ctx.load('clangxx')

'''------------------------------------------
                  Configure
   ---------------------------------------'''
def configure(ctx):
    ctx.load('clangxx')
    configure_features(ctx)

def configure_features(ctx):
    ctx.check(header_name='linux/input.h')
    ctx.check(header_name='unistd.h')
    ctx.check(header_name='sys/types.h')
    ctx.check(header_name='sys/stat.h')
    ctx.check(header_name='fcntl.h')
    ctx.check(header_name='termios.h')
    ctx.check(header_name='errno.h')

def configure_compiler(ctx):
    warnings = ['-Wall', '-Wextra']
    general  = ['-g', '-O2', '-std=c++1z']
    includes = ['-I%s/src' % ctx.path.abspath()]

    ctx.env.append_unique('CXXFLAGS', warnings + general + includes)

'''------------------------------------------
                   Build
   ---------------------------------------'''
def build(ctx):
    build_engine(ctx)
    build_program(ctx)
    build_move_binary(ctx)


def build_engine(ctx):
    ctx.objects(features='cxx',
                source=ctx.path.ant_glob('src/mpe/**/*.cpp'),
                target='mpe_engine')

def build_program(ctx):
    ctx.program(features='cxx',
                source=ctx.path.ant_glob('src/frontend/term/*.cpp'),
                target='bin/mptet',
                use='mpe_engine')

def build_move_binary(ctx):
    ctx(name='copy-mptet',
        rule='cp -f ${SRC} ${TGT}',
        source=ctx.path.get_bld().make_node('bin/mptet'),
        target=ctx.path.make_node('bin/mptet')
    )
