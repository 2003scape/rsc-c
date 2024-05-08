#!/bin/bash
#This script probably works! If not, you might need to manually install it, likely following the instructions found here: https://emscripten.org/docs/getting_started/downloads.html#installation-instructions-using-the-emsdk-recommended
cd ~
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh