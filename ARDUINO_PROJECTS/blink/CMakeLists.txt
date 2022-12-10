cmake_minimum_required(VERSION 2.8)
set(CMAKE_TOOLCHAIN_FILE /home/evg/arduino-cmake-master/cmake/ArduinoToolchain.cmake)
project(blink C CXX)
include_directories(include)

set(POJECT_NAME blink)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_OBJCOPY /usr/bin/avr-objcopy)
set(CMAKE_AVRDUDE avrdude)
set(CWARN "-c -g -Os -w -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects")
set(CMCU "-MMD -mmcu=atmega328p")
set(COPT "-Os")
set(CDEFS "-DF_CPU=16000000L -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR")
set(ARDUINO_DIR /usr/share/arduino/hardware/arduino)

set(CFLAGS "${CMCU} ${CDEFS} ${COPT} ${CWARN}")
set(CMAKE_CXX_FLAGS ${CFLAGS})

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(LINKER_FLAGS "SHELL: \
    -w -Os -Wl,--gc-sections -mmcu=atmega328p \
    ")
include_directories(
    include
    ${ARDUINO_DIR}/cores/arduino
    ${ARDUINO_DIR}/variants/standard
    ${ARDUINO_DIR}/libraries
    /usr/avr/include
    /lib/avr/include
)

set(SOURCE src/blink.cpp)
set(HEADER blink.h)
add_executable(${PROJECT_NAME} ${SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(${PROJECT_NAME} PUBLIC
 -L../../lib -lcore
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    )

#add_custom_target(download
#        COMMAND /usr/bin/avr-objcopy -O ihex -j .text -j .data -S blink blink.hex
##        COMMAND ${CMAKE_AVRDUDE} -patmega8 -carduino -P/dev/ttyUSB0 -b19200 -D -Uflash:w:./${PROJECT_NAME}.hex:i
#        )
