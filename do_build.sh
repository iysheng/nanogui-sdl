#!/bin/sh

export SDL2DIR=/opt/red_aarch64/sdl2
export SDL2IMAGEDIR=/opt/red_aarch64/sdl2_image
export SDL2TTFDIR=/opt/red_aarch64/sdl2_ttf

cmake -DCMAKE_TOOLCHAIN_FILE=../cross.txt ..
