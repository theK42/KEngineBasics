cmake_minimum_required(VERSION 3.10)

#set the project name
project(KEngine)

add_library(KEngineBasics STATIC
Input.h
Input.cpp
Audio.h
Audio.cpp
UIView.h
UIView.cpp
UITextView.h
UITextView.cpp
Renderers.h
Renderers.cpp
UIViewFactory.h
UIViewFactory.cpp
)

function(add_submodule submodule)
    message("adding submodule ${submodule}")
    cmake_path(ABSOLUTE_PATH submodule OUTPUT_VARIABLE absolute_submodule)
    if (NOT EXISTS ${absolute_submodule})
        message("submodule ${submodule} missing, attempting update")

	set (git_cmd "git")
	#set (git_arg "submodule update --init --recursive -- ${submodule}")
	set (git_arg "submodule update")
	execute_process(COMMAND ${git_cmd} submodule update --init --recursive -- ${submodule}
  			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  			RESULT_VARIABLE git_result
  			OUTPUT_VARIABLE git_out)
	
	message(STATUS "git[${git_result}]: ${git_out}")
    endif()
    add_subdirectory(${submodule})
endfunction()


target_compile_features(KEngineBasics PRIVATE cxx_std_17) 
target_link_libraries(KEngineBasics PUBLIC KEngineCore KEngine2D)

option(KENGINE_BASICS_USE_OPENGL "Whether to include support for OpenGL" ON)

if (KENGINE_BASICS_USE_OPENGL)
    target_compile_definitions(KEngineBasics PUBLIC KENGINE_USE_OPENGL)
	target_link_libraries(KEngineBasics PUBLIC KEngineOpenGL)
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-s USE_SDL=2 -s USE_SDL_MIXER=2")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    #set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
	FIND_PACKAGE(SDL2 QUIET)
	IF (${SDL2_FOUND})
		MESSAGE(STATUS "Found SDL.")
	ELSE (${SDL2_FOUND})
		MESSAGE(STATUS "Could not locate SDL, using submodule")
		add_submodule(SDL)
		set(sdlpath SDL)
		cmake_path(ABSOLUTE_PATH sdlpath OUTPUT_VARIABLE absolute_sdl2)
		set(SDL2_DIR ${absolute_sdl2} CACHE PATH FORCE "where SDL is")
		find_library(SDL2_LIBRARY NAMES SDL2 HINTS SDL ENV SDL2_DIR)
	ENDIF (${SDL2_FOUND})

	FIND_PACKAGE(SLD2_mixer QUIET)
	IF (${SDL2_mixer_FOUND})
		MESSAGE(STATUS "Found SDL Mixer.")
	ELSE (${SDL2_mixer_FOUND})
		MESSAGE(STATUS "Could not locate SDL, using submodule")
		add_submodule(SDL_mixer)
	ENDIF (${SDL2_mixer_FOUND})
	target_link_libraries(KEngineBasics PUBLIC SDL2 SDL2main SDL2_mixer)
endif()

target_include_directories(KEngineBasics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})