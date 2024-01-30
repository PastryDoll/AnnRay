# !/bin/sh

# set -xe

FLAGS="-g -std=c++11 -O3 -Wall -Wno-deprecated-declarations -Wno-unused-but-set-variable"

FRAMEWORKS="-framework CoreVideo -framework IOKit -framework Cocoa 
-framework GLUT -framework OpenGL"

LIBS="./libraylib.a"

INCLUDE_PATHS="-I./lib"

clang++ $FLAGS $FRAMEWORKS rlib_annray.cpp -o AnnRay $INCLUDE_PATHS $LIBS