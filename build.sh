#!/bin/sh

TARGET="exe"
LIBS="-lX11 -lm -lGL"
FLAGS="-std=c99 -g -Wall -Wextra"

# gcc -std=c99 -g -Wall -Wextra -lX11 -lm -lGL -o exe $1

gcc $FLAGS -o $TARGET $1 $LIBS
