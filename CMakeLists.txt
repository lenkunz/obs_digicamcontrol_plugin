project(digicamcontrol-plugin)

set(digicamcontrol-plugin_SOURCES
      digicamcontrol.cpp
      digicamcontrol-source.cpp
      digicamcontrol-source.hpp
      device-jpeg.cpp
      device-jpeg.hpp
      device-camera.cpp
      device-camera.hpp
      utils.cpp
      utils.hpp
      image.cpp
      image.hpp
      )

add_library(digicamcontrol-plugin MODULE
      ${digicamcontrol-plugin_SOURCES})
target_link_libraries(digicamcontrol-plugin
      libobs)

if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
	set(USE_STANDALONE_LIB true CACHE BOOL "enable standalonelib compilation")
	if(USE_STANDALONE_LIB)
		target_compile_definitions(digicamcontrol-plugin PRIVATE USE_STANDALONELIB=1)
	endif()
endif()

find_package(Libcurl REQUIRED)
include_directories(${LIBCURL_INCLUDE_DIRS})
target_link_libraries(digicamcontrol-plugin ${LIBCURL_LIBRARIES})

if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
	target_compile_definitions(digicamcontrol-plugin PRIVATE ENVIRONMENT32=1)

	set(LibJpegTurbo "not found" CACHE FILEPATH "Path to libjepeg-turbo main directory")
	include_directories(${LibJpegTurbo}/include)
	target_link_libraries(digicamcontrol-plugin $ENV{LibJpegTurbo}/lib/turbojpeg.lib)
else()
	target_compile_definitions(digicamcontrol-plugin PRIVATE ENVIRONMENT64=1)

	set(LibJpegTurbo64 "not found" CACHE FILEPATH "Path to libjepeg-turbo main directory (64bit)")
	include_directories(${LibJpegTurbo64}/include)
	target_link_libraries(digicamcontrol-plugin $ENV{LibJpegTurbo64}/lib/turbojpeg.lib)
endif()

target_link_libraries(digicamcontrol-plugin "legacy_stdio_definitions.lib" "Pathcch.lib")

install_obs_plugin_with_data(digicamcontrol-plugin data)
