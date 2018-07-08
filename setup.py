import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

__version__ = '0.0.1'

this_dir = os.path.dirname(os.path.abspath(__file__))

ext_modules = [
    Extension(
        'gumbo._gumbo',
        sources= [
            'src/gumbo/attribute.c',
            'src/gumbo/char_ref.c',
            'src/gumbo/error.c',
            'src/gumbo/parser.c',
            'src/gumbo/string_buffer.c',
            'src/gumbo/string_piece.c',
            'src/gumbo/tag.c',
            'src/gumbo/tokenizer.c',
            'src/gumbo/utf8.c',
            'src/gumbo/util.c',
            'src/gumbo/vector.c',
            'src/bindings/wrappers.cpp',
            'src/bindings/gumbo_py.cpp',
            ],
        include_dirs=[
            os.path.join(this_dir, 'src'),
        ],
        language='c++'
    ),
]


# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile
    with tempfile.NamedTemporaryFile('w', suffix='.cpp') as f:
        f.write('int main (int argc, char **argv) { return 0; }')
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except setuptools.distutils.errors.CompileError:
            return False
    return True


def cpp_flag(compiler):
    """Return the -std=c++[11/14] compiler flag.
    The c++14 is prefered over c++11 (when it is available).
    """
    if has_flag(compiler, '-std=c++14'):
        return '-std=c++14'
    elif has_flag(compiler, '-std=c++11'):
        return '-std=c++11'
    else:
        raise RuntimeError('Unsupported compiler -- at least C++11 support '
                           'is needed!')


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': [],
    }

    if sys.platform == 'darwin':
        c_opts['unix'] += ['-stdlib=libc++', '-mmacosx-version-min=10.7']

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, '-fvisibility=hidden'):
                opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)


setup(
    name='gumbo-python',
    version=__version__,
    author='Roman Miroshnychenko',
    author_email='roman1972@gmail.com',
    url='https://github.com/romanvm/gumbo-python',
    description='Python bindings for Gumbo HTML5 parser',
    long_description='',
    packages=['gumbo'],
    ext_modules=ext_modules,
    setup_requires=['pytest-runner'],
    install_requires=[],
    # test_suite='tests_gumbo',
    tests_require=['pytest', 'bs4'],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)
