# re-build the _yuv .c files from PNGs with
# https://github.com/misterhat/rgb-to-yuv2

# cd wii
# node ~/src/rgb-to-yuv2/index.js rsc_background.png
# node ~/src/rgb-to-yuv2/index.js rsc_type.png
# node ~/src/rgb-to-yuv2/index.js arrow.png
# cp rsc_{backgroud,type}_yuv.* arrow_yuv* ../src/wii
# cd ..

source /etc/profile.d/devkit-env.sh
make -f Makefile.wii

mkdir mudclient-177-wii
cp mudclient.dol mudclient-177-wii/boot.dol
cp wii/icon.png wii/meta.xml mudclient-177-wii
zip -r mudclient-177-wii.zip mudclient-177-wii
