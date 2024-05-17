source /etc/profile.d/devkit-env.sh

make -f Makefile.3ds clean && make -f Makefile.3ds

# below is optional to build .cia

./bannertool makebanner -i ./3ds/icon.png -a ./3ds/advance.wav -o \
    ./3ds/banner.bnr

./bannertool makesmdh -s "RuneScape" -l "RuneScape Classic" \
    -p "Jagex Ltd. & Zorian Medwid" -i ~/Pictures/3ds-2003scape-logo.png  \
    -o icon.icn

./makerom -f cia -o mudclient.cia \
    -DAPP_ROMFS=./cache  -DAPP_ENCRYPTED=false -rsf ./3ds/rsc.rsf -target t \
    -exefslogo -elf ./rsc-c.elf -icon ./3ds/icon.icn -banner ./3ds/banner.bnr
