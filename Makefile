# Copyright 2016 - Alidron's authors
#
# This file is part of Alidron.
#
# Alidron is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Alidron is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Alidron.  If not, see <http://www.gnu.org/licenses/>.

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
