cmake_minimum_required(VERSION 3.20)

project(emulator8080 C)

set(SOURCES 
    shell.c input.c state.c file_util.c sound.c display.c disassembler.c logger.c memory.c)

set(HEADERS 
    state.h file_util.h sound.h input.h display.h disassembler.h logger.h memory.h)

set(ARGS ${CMAKE_SOURCE_DIR}/rom/invaders.rom)
set(DIAG_PATH ${CMAKE_SOURCE_DIR}/rom/CPUTEST.COM)
set(EXER_PATH ${CMAKE_SOURCE_DIR}/rom/8080EXER.COM)
set(EXM_PATH ${CMAKE_SOURCE_DIR}/rom/8080EXM.COM)
set(PRE_PATH ${CMAKE_SOURCE_DIR}/rom/8080PRE.COM)
set(EX1_PATH ${CMAKE_SOURCE_DIR}/rom/8080EX1.COM)
set(CPUDIAG_PATH ${CMAKE_SOURCE_DIR}/rom/cpudiag.bin)


add_executable(emulate8080 ${SOURCES} ${HEADERS})

target_compile_features(emulate8080 PRIVATE c_std_17)
target_compile_definitions(emulate8080 PRIVATE -DSOUND_FILES="${CMAKE_SOURCE_DIR}/rom/sounds")
target_compile_definitions(emulate8080 PRIVATE -DROM_PATH="${ARGS}")
target_compile_definitions(emulate8080 PRIVATE -DCPUTEST_PATH="${DIAG_PATH}")
target_compile_definitions(emulate8080 PRIVATE -DEXER_PATH="${EXER_PATH}")
target_compile_definitions(emulate8080 PRIVATE -DPRE_PATH="${PRE_PATH}")
target_compile_definitions(emulate8080 PRIVATE -DEXM_PATH="${EXM_PATH}")
target_compile_definitions(emulate8080 PRIVATE -DDIAG_PATH="${DIAG_PATH}")
target_compile_definitions(emulate8080 PRIVATE -DCPUDIAG_PATH="${CPUDIAG_PATH}")


#target_compile_options(emulate8080 PRIVATE ${ARGS}) 

if(WIN32)

	set(DIRECTORY PROPERTY VS_STARTUP_PROJECT emulator8080)
	message("Generating VS Project files")
	source_group("Header Files" FILES ${HEADERS})
	#set_target_properties(emulate8080 PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS ${ARGS}) 

else()
	#need to check still...
	target_include_directories(emulate8080 PRIVATE ${ALLEGRO_INCLUDE_DIRS})
	target_link_libraries(emulate8080 PRIVATE ${ALLEGRO_LIBRARIES})

endif()
