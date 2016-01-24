all: demo

ifeq ($(TARGET),)
TARGET=stm32nucleo-spirit1
endif

APPDIRS += contiki-zmtp/apps
APPS += zmtp

CONTIKI=contiki-zmtp/contiki
CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include

bin: demo.stm32nucleo-spirit1
	arm-none-eabi-objcopy -O binary demo.stm32nucleo-spirit1 demo.bin
