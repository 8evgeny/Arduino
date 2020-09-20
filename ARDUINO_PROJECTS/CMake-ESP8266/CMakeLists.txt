cmake_minimum_required(VERSION 2.8)
project( esp8266 CXX)

#set_target_properties(esp8266 PROPERTIES LINKER_LANGUAGE CXX)
#set( CMAKE_CXX_FLAGS_RELEASE "-O2 -g3 -DPOSIX -DNDEBUG -D_NDEBUG -Wall -pedantic -Wno-long-long -Wno-unknown-pragmas -lpthread -rdynamic" CACHE STRING "Release options." FORCE )
set(ESP8266_DIR "/home/jhon/.arduino15/packages/esp8266/hardware/esp8266/2.7.4")

set(esp8266_SRC "/home/jhon/.arduino15/packages/esp8266/hardware/esp8266/2.7.4/libraries")
set(arduino_HOME "/home/jhon/.arduino15/packages/arduino/hardware/avr/1.8.3/cores/arduino")
include_directories(include src )
FILE(GLOB SRC_FILES
    src/*
    include/*
    ${ESP8266_DIR}/cores/ecp8266/*.c
    ${ESP8266_DIR}/cores/ecp8266/*.cpp
    ${ESP8266_DIR}/cores/ecp8266/*.h
    ${ESP8266_DIR}/variants/di_mini/*.h
    /home/jhon/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506/xtensa-lx106-elf/include/*.h
    )
include_directories(

${ESP8266_DIR}/cores/esp8266
${ESP8266_DIR}/libraries
${ESP8266_DIR}/variants/d1_mini
${ESP8266_DIR}/libraries/ESP8266WiFi/src
${ESP8266_DIR}/libraries/ESP8266mDNS/src
${ESP8266_DIR}/libraries/ArduinoOTA
${ESP8266_DIR}/tools/sdk/include
/home/jhon/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506/xtensa-lx106-elf/include
${arduino_HOME}
${CMAKE_SOURCE_DIR}
)

#file( GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.hpp" "*.cpp" "*.ino" )
#add_executable( ${PROJECT_NAME} ${SOURCES} )

add_executable(${PROJECT_NAME}  ${SRC_FILES})
