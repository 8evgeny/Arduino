# Корневой каталог исходников Arduino Core
ARDUINO_DIR=/usr/share/arduino/hardware/archlinux-arduino/avr
# Выбираем целевой контроллер (Arduino Uno, Nano, Mini)
ARDUINO_MCU=atmega328p
# Частота тактирования контроллера
ARDUINO_FCPU = 16000000L

# Ни гуи, ни ядра Qt нам не надо!
QT -= gui core
CONFIG -= qt

# Целевой каталог и имя библиотеки
DESTDIR = ../lib
TARGET = core

CONFIG += staticlib
# Шаблон проекта
TEMPLATE = lib

# Подключаем заголовочные файлы
INCLUDEPATH += $$ARDUINO_DIR/cores/arduino
INCLUDEPATH += $$ARDUINO_DIR/variants/standard
INCLUDEPATH += $$ARDUINO_DIR/libraries
INCLUDEPATH += /usr/avr/include

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

#задаем компоновщик и его ключи
QMAKE_LINK = /usr/bin/avr-gcc
QMAKE_LFLAGS = -w -Os -Wl,--gc-sections -mmcu=$$ARDUINO_MCU
QMAKE_LIBS = -lm


# Заголовки Arduino Core
HEADERS += $$files($$ARDUINO_DIR/cores/arduino/*.h)
HEADERS += $$files($$ARDUINO_DIR/variants/standard/*.h)

# Исходники Arduino Core
SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.c)
SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.cpp)
