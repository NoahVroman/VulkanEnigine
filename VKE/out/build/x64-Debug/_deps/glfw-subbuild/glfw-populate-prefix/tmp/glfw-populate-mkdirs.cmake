# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-src"
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-build"
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix"
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/tmp"
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src"
  "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/DAE/2de jaar/Semester 2/GP2/VKE/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
