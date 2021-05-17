# -*- coding: utf-8 -*-
from __future__ import print_function
from os import sys

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.', file=sys.stderr)
    print('Please run:', file=sys.stderr)
    print('', file=sys.stderr)
    print('  python -m pip install scikit-build')
    sys.exit(1)

setup(
    name='itk-higherorderaccurategradient',
    version='1.1.1',
    author='Matthew M. McCormick',
    author_email='matt.mccormick@kitware.com',
    packages=['itk'],
    package_dir={'itk': 'itk'},
    download_url=r'https://github.com/InsightSoftwareConsortium/ITKHigherOrderAccurateGradient',
    description=r'ITK filters to estimate higher order accurate derivative and gradients',
    long_description='itk-higherorderaccurategradient provides higher order '
                     'accurate derivative and gradient image filters.\n'
                     'Please refer to:\n'
                     'M. McCormick, "Higher Order Accurate Derivative and Gradient Calculation in ITK.", '
                     'Insight Journal, July-December 2010, http://hdl.handle.net/10380/3231',
    classifiers=[
        "License :: OSI Approved :: Apache Software License",
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Healthcare Industry",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Medical Science Apps.",
        "Topic :: Scientific/Engineering :: Information Analysis",
        "Topic :: Software Development :: Libraries",
        "Operating System :: Android",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Operating System :: Unix",
        "Operating System :: MacOS"
        ],
    license='Apache',
    keywords='ITK Higher-order Derivative Gradient',
    url=r'https://github.com/InsightSoftwareConsortium/ITKHigherOrderAccurateGradient',
    install_requires=[
        r'itk>=v5.2.0.post2'
    ]
    )
