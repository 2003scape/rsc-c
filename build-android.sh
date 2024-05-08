#!/bin/bash
#First, you should probably install Android SDK and Android NDK. 
#You can likely install Android files with apt: sudo apt install android-sdk-build-tools android-sdk 
#You may have to manually download Android commandline-tools and Android NDK and extract them.
export ANDROID_HOME="/home/$USER/android/"
export ANDROID_NDK_HOME="/home/$USER/android/android-ndk-r24"
cd org.scape2003.mudclient/
./gradlew assembleDebug