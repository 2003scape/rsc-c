#!/bin/sh
#First, you should probably install Android SDK and Android NDK. 
#You can likely install Android files with apt: sudo apt install android-sdk-build-tools android-sdk 
#You may have to manually download Android commandline-tools and Android NDK and extract them.
if [ -e /usr/lib/android-sdk ]; then
	export ANDROID_HOME=/usr/lib/android-sdk
	if ! [ -e /usr/lib/android-sdk/licenses/google-sdk-license ]; ten
		git clone https://github.com/Shadowstyler/android-sdk-licenses.git
		sudo cp -p android-sdk-licenses/*-license /usr/lib/android-sdk/licenses
	fi
else
	export ANDROID_HOME="/home/$USER/android/"
	export ANDROID_NDK_HOME="/home/$USER/android/android-ndk-r24"
fi
cd org.scape2003.mudclient/
./gradlew assembleDebug
