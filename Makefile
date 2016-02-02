# Copyright (c) 2015-2016 Contributors as noted in the AUTHORS file
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
