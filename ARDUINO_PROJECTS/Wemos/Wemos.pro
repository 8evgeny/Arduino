#$ sudo pacman -S qtcreator
#Кроме того, нам понадобятся пакеты, касающиеся самой Arduino
#$ sudo pacman -S arduino arduino-avr-core
#нам понадобятся ещё компилятор, ассемблер, компоновщик и стандартная библиотека C для AVR, поэтому ставим и их
#$ sudo pacman -S avr-gcc avr-binutils avr-libc
#Отладчик и эмулятор , поэтому установим ещё такие пакеты
#$ sudo pacman -S avr-gdb simavr

# Определяем переменные окружения сборки

# Корневой каталог исходников Arduino Core
ARDUINO_DIR=/usr/share/arduino/hardware/archlinux-arduino/avr/
# Выбираем целевой контроллер (Arduino Uno, Nano, Mini)
ARDUINO_MCU=atmega328p
# Частота тактирования контроллера
ARDUINO_FCPU = 16000000L

  # Корневой каталог библиотек Arduino
   ARDUINO_LIB=/root/.arduino15/pakages


# Ни гуи, ни ядра Qt нам не надо!
QT -= gui core
CONFIG -= qt

# Шаблон проекта - приложение, будет собираться исполняемый файл формата ELF
TEMPLATE = app

#Задаем каталог для собранного бинарника и его имя
DESTDIR = ../bin
TARGET = Wemos_led-blink

#Дальше подключим директории поиска заголовочных файлов
# Подключаем заголовочные файлы
INCLUDEPATH += $$ARDUINO_DIR/cores/arduino

  INCLUDEPATH += $$ARDUINO_DIR/cores/arduino/esp8266
  INCLUDEPATH += $$ARDUINO_DIR/cores/arduino/4.8.2
  INCLUDEPATH += $$ARDUINO_DIR/cores/arduino/4.8.2/xtensa-lx106-elf

  INCLUDEPATH += /usr/share/arduino/hardware/archlinux-arduino/avr/firmwares/wifishield/wifiHD/src/SOFTWARE_FRAMEWORK/SERVICES/LWIP/lwip-1.3.2/src/include
  INCLUDEPATH += /root/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506/xtensa-lx106-elf/include/c++/4.8.2
  INCLUDEPATH += /root/.arduino15/packages/esp8266/hardware/esp8266/2.7.4/tools/sdk/include

INCLUDEPATH += $$ARDUINO_DIR/variants/standard
INCLUDEPATH += $$ARDUINO_DIR/libraries
INCLUDEPATH += /usr/avr/include

# Домашний каталог библиотек
#  ARDUINO_HOME=/home/jhon/Arduino/libraries
#  ARDUINO_CORE=/home/jhon/Arduino/cores
#  ARDUINO_C++=/home/jhon/Arduino
#  INCLUDEPATH += $$ARDUINO_HOME/ESP8266WiFi/src
#  INCLUDEPATH += $$ARDUINO_HOME/ESP8266mDNS/src
#  INCLUDEPATH += $$ARDUINO_HOME/ArduinoOTA
#  INCLUDEPATH += $$ARDUINO_CORE/esp8266
#  INCLUDEPATH += $$ARDUINO_C++/4.8.2/xtensa-lx106-elf

  INCLUDEPATH += $$ARDUINO_LIB/esp8266/hardware/esp8266/2.7.4/libraries/ESP8266WiFi/src


#Задаем компилятор C и его ключи
QMAKE_CC = /usr/bin/avr-gcc
QMAKE_CFLAGS += -c -g -Os -w -ffunction-sections -fdata-sections
QMAKE_CFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
#и компилятор C++ и его ключи
QMAKE_CXX = /usr/bin/avr-g++
QMAKE_CXXFLAGS += -c -g -Os -w  -ffunction-sections -fdata-sections
QMAKE_CXXFLAGS += -fno-exceptions -fno-threadsafe-statics
QMAKE_CXXFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CXXFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR

#задаем компоновщик и его ключи
QMAKE_LINK = /usr/bin/avr-gcc
QMAKE_LFLAGS = -w -Os -Wl,--gc-sections -mmcu=$$ARDUINO_MCU
QMAKE_LIBS = -lm

#Настраиваем постобработку ELF-файла, с целью перекрутить его в Intel HEX для последующей прошивки в плату
QMAKE_POST_LINK += /usr/bin/avr-objcopy -O ihex -j .text -j .data -S ${TARGET} ${TARGET}.hex

# Заголовки Arduino Core
HEADERS += $$files($$ARDUINO_DIR/cores/arduino/*.h)
HEADERS += $$files($$ARDUINO_DIR/variants/standard/*.h)

  HEADERS += $$files($$ARDUINO_LIB/arduino/hardware/avr/1.8.3/cores/arduino/*.h)
  HEADERS += $$files($$ARDUINO_LIB/esp8266/hardware/esp8266/2.7.4/cores/esp8266/*.h)
  HEADERS += $$files($$ARDUINO_LIB/esp8266/hardware/esp8266/2.7.4/libraries/ESP8266WiFi/src/*.h)

# Исходники Arduino Core
SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.c)
SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.cpp)

  SOURCES += $$files($$ARDUINO_LIB/arduino/hardware/avr/1.8.3/cores/arduino/*.c)
  SOURCES += $$files($$ARDUINO_LIB/arduino/hardware/avr/1.8.3/cores/arduino/*.cpp)
  SOURCES += $$files($$ARDUINO_LIB/esp8266/hardware/esp8266/2.7.4/cores/esp8266/*.c)
  SOURCES += $$files($$ARDUINO_LIB/esp8266/hardware/esp8266/2.7.4/cores/esp8266/*.cpp)
  SOURCES += $$files($$ARDUINO_LIB/esp8266/hardware/esp8266/2.7.4/libraries/ESP8266WiFi/src/*.cpp)

#Заголовки проекта

INCLUDEPATH += ./include
HEADERS += $$files(./include/*.h)

# Исходники проекта
SOURCES += $$files(./src/*.cpp)