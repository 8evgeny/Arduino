# Целевой каталог и имя библиотеки
DESTDIR = ../lib
TARGET = core

# Подключаем заголовочные файлы
INCLUDEPATH += $$ARDUINO_DIR/cores/arduino
INCLUDEPATH += $$ARDUINO_DIR/variants/standard
INCLUDEPATH += $$ARDUINO_DIR/../../libraries
INCLUDEPATH += $$ARDUINO_DIR/hardware/arduino
INCLUDEPATH += /usr/avr/include
INCLUDEPATH += /usr/avr/include
INCLUDEPATH += /usr/share/arduino/hardware/tools/avr/lib/x86_64-linux-gnu/qtcreator/plugins
# Настройки компилятора C
QMAKE_CC = /usr/bin/avr-gcc
QMAKE_CFLAGS += -c -g -Os -w -ffunction-sections -fdata-sections
QMAKE_CFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

# Настройки компилятора C++
QMAKE_CXX = /usr/bin/avr-g++
QMAKE_CXXFLAGS += -c -g -Os -w  -ffunction-sections -fdata-sections
QMAKE_CXXFLAGS += -fno-exceptions -fno-threadsafe-statics
QMAKE_CXXFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CXXFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

# Заголовки Arduino Core
HEADERS += $$files($$ARDUINO_DIR/cores/arduino/*.h)
HEADERS += $$files($$ARDUINO_DIR/variants/standard/*.h)

# Исходники Arduino Core
SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.c)
SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.cpp)
