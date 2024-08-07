# !/bin/sh

# set -xe

MAIN_FILE=../src/rlib_annray.cpp
OUTPUT_FILE=AnnRay

FLAGS="-g -std=c++11 -O0 -Wall -Wno-deprecated-declarations -Wno-unused-but-set-variable" # -Wno-c++11-narrowing"

FRAMEWORKS="-framework CoreVideo -framework IOKit -framework Cocoa 
-framework GLUT -framework OpenGL"

LIBS="../external/raylib/src/libraylib.a"

INCLUDE_PATHS="-I../external/raylib/src/ -I../external/raygui"

clang++ $FLAGS $FRAMEWORKS $MAIN_FILE -o $OUTPUT_FILE $INCLUDE_PATHS $LIBS