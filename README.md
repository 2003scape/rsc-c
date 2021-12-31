# rsc-c

runescape classic client ported to C.

## build (desktop)

install [libsdl2-dev](https://packages.debian.org/sid/libsdl2-dev).

    $ make
    $ ./mudclient

# build (wii)

install the
[wii-dev package in devkitPro](https://devkitpro.org/wiki/Getting_Started)
(optionally [dolphin-emu](https://packages.debian.org/sid/dolphin-emu) to test
locally).

    $ source /etc/profile.d/devkit-env.sh
    $ make -f Makefile.wii
    $ dolphin-emu ./mudclient.dol # optional - test the game in an emulator
    $ cp mudclient.dol wii/boot.dol

copy the `./wii/` directory to the `./apps`/ directory in your SD card (you can
also rename it if you want).

## license
Copyright 2021  2003Scape Team

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along
with this program. If not, see http://www.gnu.org/licenses/.
