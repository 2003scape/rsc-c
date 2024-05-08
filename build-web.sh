#/bin/bash
bash ~/emsdk/emsdk_env.sh
make -f Makefile.emscripten
#Now, you should be able to run it with the following:
# python3 -m http.server 1337 # @ http://localhost:1337/mudclient.html