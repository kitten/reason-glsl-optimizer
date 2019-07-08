cmake_minimum_required(VERSION 2.8)

set(CMAKE_MACOSX_RPATH 1)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/mesa
)

file(GLOB_RECURSE UTIL_SOURCE ./util/*.c)
file(GLOB_RECURSE UTIL_HEADER ./util/*.h)

file(GLOB_RECURSE MESA_MAIN_SOURCE ./mesa/main/*.c)
file(GLOB_RECURSE MESA_MAIN_HEADER ./mesa/main/*.h)

file(GLOB_RECURSE MESA_PROG_SOURCE ./mesa/program/*.c)
file(GLOB_RECURSE MESA_PROG_HEADER ./mesa/program/*.h)

file(GLOB_RECURSE GLCPP_SOURCE ./glsl/glcpp/*.c)
file(GLOB_RECURSE GLCPP_HEADER ./glsl/glcpp/*.h)

file(GLOB_RECURSE GLSL_CPP_SOURCE ./glsl/*.cpp)
file(GLOB_RECURSE GLSL_C_SOURCE ./glsl/*.c)
file(GLOB_RECURSE GLSL_HEADER ./glsl/*.h)

file(GLOB_RECURSE ROOT_SOURCE ./glsloptimizer.c)
file(GLOB_RECURSE ROOT_HEADER ./glsloptimizer.h)

add_compile_options(
  -Os
  -DNDEBUG=1
  -DYYDEBUG=0
  -Wno-return-type
  -Wno-deprecated-register
  -fno-threadsafe-statics
)

add_library(libglsloptimizer STATIC
	${ROOT_SOURCE} ${ROOT_HEADER}
	${UTIL_SOURCE} ${UTIL_HEADER}
	${MESA_MAIN_SOURCE} ${MESA_MAIN_HEADER}
	${MESA_PROG_SOURCE} ${MESA_PROG_HEADER}
	${GLCPP_SOURCE} ${GLCPP_HEADER}
	${GLSL_CPP_SOURCE} ${GLSL_C_SOURCE} ${GLSL_HEADER})

add_library(dllglsloptimizer SHARED
	${ROOT_SOURCE} ${ROOT_HEADER}
	${UTIL_SOURCE} ${UTIL_HEADER}
	${MESA_MAIN_SOURCE} ${MESA_MAIN_HEADER}
	${MESA_PROG_SOURCE} ${MESA_PROG_HEADER}
	${GLCPP_SOURCE} ${GLCPP_HEADER}
	${GLSL_CPP_SOURCE} ${GLSL_C_SOURCE} ${GLSL_HEADER})

set_target_properties(libglsloptimizer PROPERTIES PREFIX "")
set_target_properties(dllglsloptimizer PROPERTIES PREFIX "")
set_target_properties(dllglsloptimizer PROPERTIES OUTPUT_NAME "dllglsloptimizer_stubs")
set_target_properties(libglsloptimizer PROPERTIES OUTPUT_NAME "libglsloptimizer_stubs")
set_target_properties(dllglsloptimizer PROPERTIES SUFFIX ".so")
set_target_properties(libglsloptimizer PROPERTIES SUFFIX ".a")
