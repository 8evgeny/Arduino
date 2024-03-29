# Определяем переменные окружения сборки
DEFINES += ARDUINO=328
# Корневой каталог исходников Arduino Core
ARDUINO_DIR=/usr/share/arduino/hardware/arduino/
# Выбираем целевой контроллер (Arduino Uno, Nano, Mini)
ARDUINO_MCU=atmega328p
# Частота тактирования контроллера
ARDUINO_FCPU = 16000000L

# Корневой каталог системных библтиотек
ARDUINO_SYS_LIB=/usr/share/arduino/libraries

# Корневой каталог сторонних библтиотек
ARDUINO_EXT_LIB=/home/evg/SOFT/Github/Arduino/libraries

# Ни гуи, ни ядра Qt нам не надо!
QT -= gui core
CONFIG -= qt

# Шаблон проекта - приложение, будет собираться исполняемый файл формата ELF
TEMPLATE = app

# Целевой каталог и имя бинарника
DESTDIR = ../../bin
TARGET = display

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
QMAKE_CXXFLAGS += -fpermissive -flto -fno-devirtualize -fno-use-cxa-atexit

# Настройки компоновщика
QMAKE_LINK = /usr/bin/avr-gcc
QMAKE_LFLAGS = -w -Os -Wl,--gc-sections -mmcu=$$ARDUINO_MCU
QMAKE_LIBS = -lm

# Постобработка
QMAKE_POST_LINK += /usr/bin/avr-objcopy -O ihex -j .text -j .data -S ${TARGET} ${TARGET}.hex

LIBS += -L../../lib -lcore
LIBS += -L../../lib -lspl

#Заголовки проекта
INCLUDEPATH += ./include
HEADERS += $$files(./include/*.h)

# Исходники проекта
SOURCES += $$files(./src/*.cpp)
