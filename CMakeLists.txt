cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)
project(HigherOrderAccurateGradient)

if(NOT ITK_SOURCE_DIR)
  find_package(ITK REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${ITK_CMAKE_DIR})
  include(ITKModuleExternal)
else()
  itk_module_impl()
endif()
