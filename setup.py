
import os
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name):
        Extension.__init__(self, name, sources=[])


class CMakeBuild(build_ext):
    def run(self):
        print('===============================================================')
        for ext in self.extensions:
            self.build_extension(ext)
        print('===============================================================')

    def build_extension(self, ext):
        print(f'Building {ext.name} ...')
        print('  (see build.log for build details)')

        build_log = open('build.log', 'w')

        # make sure the build temp directory is created
        os.makedirs(self.build_temp, exist_ok=True)

        # cmake configuration
        extdir = os.path.abspath(os.path.dirname(
            self.get_ext_fullpath(ext.name)))
        config = 'Debug' if self.debug else 'Release'
        subprocess.check_call(
            ['cmake',
                project_root,
                f'-DCMAKE_BUILD_TYPE={config}',
                f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}/$<0:>'],
            cwd=self.build_temp,
            stdout=build_log)

        # build
        subprocess.check_call(
            ['cmake', '--build', '.', f'-j{os.cpu_count()}'],
            cwd=self.build_temp,
            stdout=build_log)


# Pre-setup: if we're running setup.py from the project's root location,
# create a sandbox subdirectory to contain all the fallout from setuptools
# (ex. *.egg-info, dist, ...)
#
# Other than avoiding the top level litter, this makes it easy to
# do a full cleanup by just deleting the .setuptools directory
#
project_root = os.path.dirname(os.path.abspath(__file__))

if os.path.abspath(os.getcwd()) == project_root:
    # create the build directory for the binding extension
    setuptools_root = os.path.abspath('.setuptools')
    os.makedirs(setuptools_root, exist_ok=True)
    os.chdir(setuptools_root)
else:
    setuptools_root = os.getcwd()

setup(
    name="nwcpp-pybind11-talk",
    version="1.0",
    author='Leonard Mosescu',
    author_email='lemo1234@gmail.com',
    description='NWCPP Pybind11 Example',
    url='https://github.com/tlemo/nwcpp_pybind11',
    license='Apache License 2.0',
    ext_modules=[CMakeExtension(name='nwcpp')],
    cmdclass=dict(build_ext=CMakeBuild),
    test_suite="tests.python",
    classifiers=[
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'License :: OSI Approved :: Apache Software License',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
    ],
    python_requires='>=3.6',
    zip_safe=False)
