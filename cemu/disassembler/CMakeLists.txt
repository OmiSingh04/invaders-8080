cmake_minimum_required(VERSION 3.20)

project(disassembler8080 C)

set(SOURCES
	run.c
	disassembler.c
)

set(HEADERS
	disassembler.h
)

add_executable(disassembler ${SOURCES} ${HEADERS})

target_include_directories(disassembler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(disassembler PRIVATE c_std_17)

if(WIN32)
	set(VS_STARTUP_PROJECT disassembler8080)
	message("Generating custom groups for VS")
	source_group("Header Files" FILES disassembler.h)
	message(${CMAKE_CURRENT_SOURCE_DIR}/disassembler.h)
	set_target_properties(disassembler PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_SOURCE_DIR}/rom/invaders.rom")
endif()

message(${CMAKE_SOURCE_DIR})
message(${CMAKE_BINARY_DIR})
