# !/bin/sh

# set -xe

MAIN_FILE=../src/controls_test_suite.c
OUTPUT_FILE=Example

FLAGS="-g -std=c++11 -O3 -Wall -Wno-deprecated-declarations -Wno-unused-but-set-variable"

FRAMEWORKS="-framework CoreVideo -framework IOKit -framework Cocoa 
-framework GLUT -framework OpenGL"

LIBS="../external/raylib/src/libraylib.a"

INCLUDE_PATHS="-I../external/raylib/src/ -I../external/raygui"

clang++ $FLAGS $FRAMEWORKS $MAIN_FILE -o $OUTPUT_FILE $INCLUDE_PATHS $LIBS