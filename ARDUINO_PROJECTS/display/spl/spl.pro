# Определяем переменные окружения сборки
DEFINES += ARDUINO=328

# Корневой каталог исходников Arduino Core
ARDUINO_DIR=/usr/share/arduino/hardware/arduino/

# Корневой каталог исходников SPL
ARDUINO_DIR=/usr/share/arduino/hardware/arduino/

# Корневой каталог системных библтиотек
ARDUINO_SYS_LIB=/usr/share/arduino/libraries

# Корневой каталог сторонних библтиотек
ARDUINO_EXT_LIB=/home/evg/SOFT/Github/Arduino/libraries

# Выбираем целевой контроллер (Arduino Uno, Nano, Mini)
ARDUINO_MCU=atmega328p
# Частота тактирования контроллера
ARDUINO_FCPU = 16000000L

# Ни гуи, ни ядра Qt нам не надо!
QT -= gui core
CONFIG -= qt

# Шаблон проекта - приложение, будет собираться исполняемый файл формата ELF
TEMPLATE = lib
CONFIG += staticlib

# Целевой каталог и имя бинарника
DESTDIR = ../../lib
TARGET = spl

# Подключаем заголовочные файлы
INCLUDEPATH += $$ARDUINO_DIR/cores/arduino
INCLUDEPATH += $$ARDUINO_DIR/variants/standard
INCLUDEPATH += $$ARDUINO_DIR/libraries
INCLUDEPATH += /usr/avr/include
INCLUDEPATH += /lib/avr/include

INCLUDEPATH += $$ARDUINO_EXT_LIB/Wire
INCLUDEPATH += $$ARDUINO_EXT_LIB/Wire/utility
INCLUDEPATH += $$ARDUINO_SYS_LIB/SPI
INCLUDEPATH += $$ARDUINO_SYS_LIB/LiquidCrystal
INCLUDEPATH += $$ARDUINO_EXT_LIB/LiquidCrystal_I2C
INCLUDEPATH += $$ARDUINO_EXT_LIB/LiquidCrystal_PCF8574/src
INCLUDEPATH += $$ARDUINO_SYS_LIB/TFT/utility
INCLUDEPATH += $$ARDUINO_EXT_LIB/Adafruit_SSD1306
INCLUDEPATH += $$ARDUINO_EXT_LIB/Adafruit_GFX_Library
INCLUDEPATH += $$ARDUINO_EXT_LIB/Adafruit_BusIO
INCLUDEPATH += $$ARDUINO_EXT_LIB/DallasTemperature
INCLUDEPATH += $$ARDUINO_EXT_LIB/LiquidCrystal_PCF8574/src
INCLUDEPATH += $$ARDUINO_EXT_LIB/OneWire
INCLUDEPATH += $$ARDUINO_EXT_LIB/OneWire/util
#INCLUDEPATH += $$ARDUINO_EXT_LIB/TM1637_Driver/src
INCLUDEPATH += $$ARDUINO_EXT_LIB/TM1637


# Настройки компилятора C
QMAKE_CC = /usr/bin/avr-gcc
QMAKE_CFLAGS += -c -g -Os -w -ffunction-sections -fdata-sections
QMAKE_CFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
QMAKE_CFLAGS += -flto -fno-fat-lto-objects

# Настройки компилятора C++
QMAKE_CXX = /usr/bin/avr-g++
QMAKE_CXXFLAGS += -c -g -Os -w  -ffunction-sections -fdata-sections
QMAKE_CXXFLAGS += -fno-exceptions -fno-threadsafe-statics
QMAKE_CXXFLAGS += -MMD -mmcu=$$ARDUINO_MCU -DF_CPU=$$ARDUINO_FCPU
QMAKE_CXXFLAGS += -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
QMAKE_CXXFLAGS += -fpermissive -flto -fno-devirtualize

# Настройки компоновщика
QMAKE_LINK = /usr/bin/avr-gcc
QMAKE_LFLAGS = -w -Os -Wl,--gc-sections -mmcu=$$ARDUINO_MCU
QMAKE_LIBS = -lm

LIBS += -L../../lib -lcore

# Заголовки Arduino SPL
HEADERS += $$files($$ARDUINO_EXT_LIB/Wire/Wire.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/Wire/utility/twi.h)
HEADERS += $$files($$ARDUINO_SYS_LIB/SPI/SPI.h)
HEADERS += $$files($$ARDUINO_SYS_LIB/LiquidCrystal/LiquidCrystal.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/LiquidCrystal_I2C/LiquidCrystal_I2C.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/LiquidCrystal_PCF8574/src/LiquidCrystal_PCF8574.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/DallasTemperature/DallasTemperature.h)
#HEADERS += $$files($$ARDUINO_EXT_LIB/TM1637_Driver/src/*.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/TM1637/TM1637Display.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/OneWire/OneWire.h)
HEADERS += $$files($$ARDUINO_EXT_LIB/OneWire/util/*.h)


#HEADERS += $$files($$ARDUINO_EXT_LIB/Adafruit_GFX_Library/*.h)
#HEADERS += $$files($$ARDUINO_EXT_LIB/Adafruit_SSD1306/*.h)
#HEADERS += $$files($$ARDUINO_EXT_LIB/Adafruit_BusIO/*.h)
#HEADERS += $$files(/usr/share/arduino/libraries/TFT/TFT.h)
#HEADERS += $$files(/usr/share/arduino/libraries/TFT/utility/*.h)

#/home/evg/SOFT/Github/Arduino/libraries/LiquidCrystal_PCF8574/src
#/home/evg/SOFT/Github/Arduino/libraries/OneWire


# Исходники Arduino Core
#SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.c)
#SOURCES += $$files($$ARDUINO_DIR/cores/arduino/*.cpp)

SOURCES += $$files($$ARDUINO_EXT_LIB/Wire/Wire.cpp)
SOURCES += $$files($$ARDUINO_EXT_LIB/Wire/utility/twi.c)
SOURCES += $$files($$ARDUINO_SYS_LIB/SPI/SPI.cpp)
SOURCES += $$files($$ARDUINO_SYS_LIB/LiquidCrystal/LiquidCrystal.cpp)
SOURCES += $$files($$ARDUINO_EXT_LIB/LiquidCrystal_I2C/LiquidCrystal_I2C.cpp)
SOURCES += $$files($$ARDUINO_EXT_LIB/DallasTemperature/DallasTemperature.cpp)
#SOURCES += $$files($$ARDUINO_EXT_LIB/TM1637_Driver/src/TM1637.cpp)
SOURCES += $$files($$ARDUINO_EXT_LIB/TM1637/TM1637Display.cpp)
SOURCES += $$files($$ARDUINO_EXT_LIB/OneWire/OneWire.cpp)
SOURCES += $$files($$ARDUINO_EXT_LIB/LiquidCrystal_PCF8574/src/LiquidCrystal_PCF8574.cpp)

#SOURCES += $$files($$ARDUINO_EXT_LIB/Adafruit_GFX_Library/*.c)
#SOURCES += $$files($$ARDUINO_EXT_LIB/Adafruit_GFX_Library/*.cpp)
#SOURCES += $$files($$ARDUINO_EXT_LIB/Adafruit_SSD1306/Adafruit_SSD1306.cpp)
#SOURCES += $$files($$ARDUINO_EXT_LIB/Adafruit_BusIO/*.cpp)
#SOURCES += $$files(/usr/share/arduino/libraries/TFT/TFT.cpp)
#SOURCES += $$files(/usr/share/arduino/libraries/TFT/utility/*.c)
#SOURCES += $$files(/usr/share/arduino/libraries/TFT/utility/*.cpp)

#/usr/share/arduino/libraries/TFT/utility
#/home/evg/SOFT/Github/Arduino/libraries/Adafruit_SSD1306
#/home/evg/SOFT/Github/Arduino/libraries/DallasTemperature
#/home/evg/SOFT/Github/Arduino/libraries/LiquidCrystal_PCF8574/src
#/home/evg/SOFT/Github/Arduino/libraries/OneWire
#/home/evg/SOFT/Github/Arduino/libraries/TM1637_Driver/src
