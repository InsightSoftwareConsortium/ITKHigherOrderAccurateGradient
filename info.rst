ITKHigherOrderAccurateGradient
==============================

.. image:: https://github.com/InsightSoftwareConsortium/ITKHigherOrderAccurateGradient/workflows/Build,%20test,%20package/badge.svg

.. image:: https://img.shields.io/pypi/v/itk-higherorderaccurategradient.svg
    :target: https://pypi.python.org/pypi/itk-higherorderaccurategradient
    :alt: PyPI

.. image:: https://img.shields.io/badge/License-Apache%202.0-blue.svg
    :target: https://github.com/InsightSoftwareConsortium/ITKHigherOrderAccurateGradient/blob/master/LICENSE)
    :alt: License

Overview
--------

This repository has InsightToolkit (ITK) filters for calculating higher order
accurate numerical derivatives and gradients from an input scalar image.

Installation
------------

To install the Python package::

  python -m pip install itk-higherorderaccurategradient

To build the C++ module, either enable the CMake option in ITK's
build configuration::

  Module_HigherOrderAccurateGradient:BOOL=ON

Or, build the module as a separate project against an ITK build tree::

  git clone https://github.com/InsightSoftwareConsortium/ITKHigherOrderAccurateGradient
  mkdir ITKHigherOrderAccurateGradient-build
  cd ITKHigherOrderAccurateGradient-build
  cmake -DITK_DIR=/path/to/ITK-build ../ITKHigherOrderAccurateGradient
  cmake --build .

License
-------

This software is distributed under the Apache 2.0 license. Please see the
*LICENSE* file for details.
