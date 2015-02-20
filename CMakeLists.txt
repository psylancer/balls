
cmake_minimum_required(VERSION 2.8.12) # http://www.cmake.org/download/

# Project name
project(balls)

# Forbidding create project for any enviroment except Windows
if(CMAKE_HOST_WIN32)
  if(EXISTS ${DX_INCLUDE})
    if(EXISTS ${DX_LIB_PATH})

      if(MSVC)
        set(IS_SUBSYSTEM_WIN32 WIN32)
      endif(MSVC)

      link_directories(${LINK_DIRECTORIES} ${DX_LIB_PATH})
      file(GLOB_RECURSE C_FILES ${CMAKE_SOURCE_DIR}/*.c)
      file(GLOB_RECURSE CPP_FILES ${CMAKE_SOURCE_DIR}/*.cpp)
      file(GLOB_RECURSE INL_FILES ${CMAKE_SOURCE_DIR}/*.inl)
      file(GLOB_RECURSE H_FILES ${CMAKE_SOURCE_DIR}/*.h)
      add_executable(${PROJECT_NAME} ${IS_SUBSYSTEM_WIN32} ${C_FILES} ${CPP_FILES} ${INL_FILES} ${H_FILES})
      target_link_libraries(${PROJECT_NAME} d3d9 d3dx9)
      target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DX_INCLUDE})

      #FLAGS
      set(FLAGS)
      if(MSVC)
        set(FLAGS "${FLAGS} /W4 /GS")
        set(LINK_FLAGS "${LINK_FLAGS} /SafeSEH /DynamicBase /NXCompat")
      else()
        set(FLAGS "${FLAGS} -flto -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unknown-pragmas")
      endif(MSVC)

      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DEFAULT_CMAKE_C_FLAGS} ${FLAGS} -std=c99")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFAULT_CMAKE_CXX_FLAGS} ${FLAGS}")
      #FLAGS

    else()
       MESSAGE("Addition link directories not set")
    endif(EXISTS ${DX_LIB_PATH})
  else()
    MESSAGE("Addition include directories not set")
  endif(EXISTS ${DX_INCLUDE})
endif(CMAKE_HOST_WIN32)
