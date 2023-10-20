from setuptools import setup, Extension
import numpy

module = Extension('fast_glb_mesh_loader',
                    sources=['load_mesh.cpp'],
                    include_dirs=[numpy.get_include()],
                    libraries=[],
                    language='c++',
                    extra_compile_args=['-std=c++17', '-Wno-unused-function', '-Wno-deprecated', '-Wno-deprecated-declarations', '-O3'])

setup(name='fast_glb_mesh_loader',
      version='0.0.1',
      description='Python Package with C++ Extension',
      install_requires=['numpy'],
      ext_modules=[module])