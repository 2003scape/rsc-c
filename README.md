# rsc-c

portable runescape classic client ported to C. supports 204 and 177
(for [openrsc](https://rsc.vet/) support) client revisions.

QR code for 3DS install:

![](./3ds-cia.png?raw=true)

### controls (wii):

* d-pad to control camera
* B for right click
* A for left click
* \- to toggle on-screen keyboard
* B for backspace whilst keyboard is open
* \+ to press enter key
* 1 to toggle F1/interlace mode

a USB keyboard and mouse can also be used.

### controls (3ds):

* d-pad/circle pad to control camera
* touch screen for mouse input
* hold L whilst tapping touch screen for right click
* tap R to toggle top screen zoom
* hold L whilst using d-pad or analogue stick to pan top screen zoom
* start to toggle on-screen keyboard
* select to toggle F1/interlace mode

## build (linux)

install [libsdl2-dev](https://packages.debian.org/sid/libsdl2-dev).

    $ make
    $ ./mudclient

distribute with `./cache` directory.

# build (windows)

    $ make -f Makefile.windows
    $ wine mudclient.exe

distribute with `./SDL2.dll` and `./cache` directory.

## build (web)

install and activate
[emscripten emsdk](https://emscripten.org/docs/getting_started/downloads.html#installation-instructions-using-the-emsdk-recommended).

    $ make -f Makefile.emscripten
    $ python3 -m http.server 1337 # @ http://localhost:1337/mudclient.html

host `mudclient.{html,data,js,wasm}` and `./cache/` directory.

## build (wii)

install
[wii-dev package in devkitPro](https://devkitpro.org/wiki/Getting_Started),
and optionally [clone rgb-to-yuv2](https://github.com/misterhat/rgb-to-yuv2) to
re-build the \_yuv `.c` files in `./src/wii/` from the PNGs in `./wii`.

use [dolphin-emu](https://packages.debian.org/sid/dolphin-emu) to test
locally. enable LLE sound for sound-effects support. on real hardware you can
also use a USB keyboard and mouse.

see `./build-wii.sh` for building for Wii.

## build (3ds)

install
[3ds-dev package in devkitPro](https://devkitpro.org/wiki/Getting_Started),
and optionally [bannertool](https://github.com/Steveice10/bannertool/) and
[makerom](https://github.com/3DSGuy/Project_CTR/tree/master/makerom) to build
an [installable `.cia` file](https://www.3dbrew.org/wiki/CIA). use
[citra](https://citra-emu.org/download/) to test locally.

see `./build-3ds.sh` for building for 3DS.

## usage

arguments:
* `[members]`
    * pass any value other than "members" to load a free to play configuration
* `[ip address]`
    * an IPv4 address where the RSC177/204 compatible server is hosted
* `[port]`
    * network port number (e.g. 43594)
* `[RSA exponent]`
* `[RSA modulus]`
    * these two are used together to encrypt passwords, unique to each server.
    they must be converted to a hexadecimal representation
    [0-padded to a multiple of eight characters](https://github.com/kokke/tiny-bignum-c/issues/14)


for the desktop version, these are passed as command-line arguments. on the
web they're passed in the URL hash separated by commas
(`http://localhost:1337/mudclient.html#free,127.0.0.1`).

## libraries used

* [tiny-bignum-c](https://github.com/kokke/tiny-bignum-c) for RSA encryption on
login/registration
* [micro-bunzip](https://landley.net/code/) for decompressing cache archives
* [libsdl2](https://www.libsdl.org/index.php) for input/output on desktop

## license
Copyright 2022  2003Scape Team

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along
with this program. If not, see http://www.gnu.org/licenses/.
