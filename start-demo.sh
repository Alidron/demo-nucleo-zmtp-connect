#!/bin/bash

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

trap 'killall' INT

killall() {
    trap '' INT TERM     # ignore INT and TERM while shutting down
    echo "**** Shutting down... ****"
    kill -TERM 0
    wait
    echo DONE
}

./demo.minimal-net &
sleep 1
ip -6 address add fdfd::1/64 dev tap0

cat
