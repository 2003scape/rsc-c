# rsc-c

portable, enhanced runescape classic client ported to C. supports 204 and 177
(for [openrsc](https://rsc.vet/) support) client revisions. original
mudclient204 java source refactored by v0rtex/xetr0v and Isaac Eggsampler.

prominent new features include:
* native ports for 3ds, switch, wii, web browser and desktop
* opengl/webgl renderer with customizable
[field of view](https://en.wikipedia.org/wiki/Field_of_view)
* dynamic resizable mode with interface scaling and new minimum resolution of
320x240
* 3D model ground items based on runescape 2 beta models
* [runescape classic wiki](https://classic.runescape.wiki/) button
* bank interface with scrollbar and search
* [...and more! ](https://github.com/2003scape/rsc-c#options)

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

### controls (switch):

* d-pad/left stick to control camera
* A for Enter
* B for Backspace
* X for Tab
* Y for Home
* L for Escape
* R to toggle fps display
* ZR is reserved for future use
* touch screen for mouse input
* hold ZL whilst tapping touch screen for right click
* + to toggle on-screen keyboard
* - to toggle F1/interlace mode

## build (linux)

install [libsdl2-dev](https://packages.debian.org/sid/libsdl2-dev). if compiling
with opengl support, also install
[libglew-dev](https://packages.debian.org/sid/libglew-dev) and
[libgl-dev](https://packages.debian.org/sid/libgl-dev).

    $ make
    $ ./mudclient

distribute with `./cache` directory.

## build (windows)

install [mingw-w64](https://packages.debian.org/sid/mingw-w64).

use [wine](https://packages.debian.org/sid/wine) to test locally.

    $ make -f Makefile.windows
    $ wine mudclient.exe

distribute with `./SDL2.dll`, `./glew32.dll` and `./cache` directory.

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

## build (switch)

install
[switch-dev package in devkitPro](https://devkitpro.org/wiki/Getting_Started),
switch-sdl2, and, switch-bzip2 to build
an [`.nro` file](https://switchbrew.org/wiki/Homebrew_Applications). use
[yuzu](https://yuzu-emu.org/) to test locally.

see `./build-switch.sh` for building for Switch.

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

## options

```
; IPv4 address with revision 177 compatible protocol support
server = 127.0.0.1
port = 43594
; Disable registration and load sounds, P2P landscape and items (requires
; restart)
members = 1
; If enabled, override members option and always show New User button
registration = 1
; Used together to encrypt passwords, Must be represented as hexadecimal string
; 0-padded to a multiple of eight characters
rsa_exponent =
rsa_modulus =
; Log out when mouse is idle
idle_logout = 0
; Remember username on login screen
remember_username = 0
; Remember password on login screen (not secure)
remember_password = 0

username =
password =

; System command to use to open the web browser (only on desktop)
browser_command = xdg-open "%s"

; Scroll panel lists, chatbox, and camera (if zoom enabled) with wheel
mouse_wheel = 1
; Hold down middle click and move mouse to rotate camera (manual mode)
middle_click_camera = 1
; Use arrow, page, home keys and mouse wheel (if enabled) to zoom
zoom_camera = 1
; Respond to the last private message with tab key
tab_respond = 1
; Use number keys to select options
option_numbers = 1
; Adds a menu with different directions to face to the minimap compass
compass_menu = 1
; Adds right click menus to trades and duels
transaction_menus = 1
; Allow inputting item amounts
offer_x = 1
; Add another button to perform the last offer x amount
last_offer_x = 1
; Add RuneScape Wiki lookup button instead of report abuse
wiki_lookup = 1

; F1 mode - only render every second scanline
interlace = 0
; Display the FPS at the bottom right of the screen
display_fps = 0
; Double the UI size but keep the scene size if window is over doubleoriginal
; size (GL only)
ui_scale = 1
; Enable multi-sampling
anti_alias = 1
; Change the field of view. scales with height by default and in
; software. About 36 degrees on the original height of 346 (GL only)
field_of_view = 360
; Show roofs unless inside buildings
show_roofs = 1
; Format large numbers with commas
number_commas = 1
; Show the remaining experience until next level in skills tab
remaining_experience = 1
; Show your total experience in the skills tab
total_experience = 1
; Show experience drops
experience_drops = 1
; Show a count of inventory items on the UI
inventory_count = 0
; Condenses item amounts with K and M and add their amounts to examine
condense_item_amounts = 1
; Also draw which item a certificate is associated with
certificate_items = 1
; Display the warning dialog near the wilderness border
wilderness_warning = 1
; Display hits and prayer bars
status_bars = 0
; Use ground item models instead of billboarded sprites
ground_item_models = 1

; Add filtering to the bank
bank_search = 1
; Adds capacity to the bank
bank_capacity = 1
; Adds total high alchemy value to the bank
bank_value = 1
; Expand bank item grid with client height
bank_expand = 1
; Use a scrollbar instead of bank pages
bank_scroll = 1
; Adds right click menus to bank items
bank_menus = 1
; Shows the inventory along with the bank interface, given enough width
bank_inventory = 1
; Maintain the selected bank slot when items change position
bank_maintain_slot = 1
```

## libraries used

* [tiny-bignum-c](https://github.com/kokke/tiny-bignum-c) for RSA encryption on
login/registration
* [micro-bunzip](https://landley.net/code/) for decompressing cache archives
* [libsdl2](https://www.libsdl.org/index.php) for input/output on desktop
* [glew](http://glew.sourceforge.net/) for runtime opengl extension loading
* [ini](https://github.com/rxi/ini) for parsing *options.ini*

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
