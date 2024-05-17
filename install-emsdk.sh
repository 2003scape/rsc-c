#!/bin/bash
#This script probably works! If not, you might need to manually install it, likely following the instructions found here: https://emscripten.org/docs/getting_started/downloads.html#installation-instructions-using-the-emsdk-recommended
#With that said, we currently need emsdk 3.1.22 in order for "Save and close" to not throw SDL2 audio errors.
cd ~
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.22
./emsdk activate 3.1.22
source ./emsdk_env.sh