#!/bin/sh
x86_64-w64-mingw32-windres icon.rc -O coff -o icon.res
x86_64-w64-mingw32-windres meta.rc -O coff -o meta.res
