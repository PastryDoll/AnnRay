#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"

#include "rlib_annray.h"

#define TEST_FOLDER_PNG "../images_samplepng/"
#define TEST_FOLDER_JPG "../test_material/images_sample"
#define PROJECT_FOLDER "../project"
#define TEST_PIC "DJI_0325.JPG"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

#define PANELWIDTH (float)280
#define IMAGEDISPLAYSIDEGAP (float)32

#include "annray_math.h"
#include "rlib_annotation_menu.cpp"

#define TESTTHUMB 0

// internal
// void GenerateThumbnails(Image PreviewImages[], Texture PreviewTextures[], FilePathList PathList)
// {   
//     for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
//         {
//             char *Path = *(PathList.paths + PathIndex);
//             PreviewImages[PathIndex] = LoadImage(Path);
//             ImageResize(&PreviewImages[PathIndex],128,128);
//             PreviewTextures[PathIndex] = LoadTextureFromImage(PreviewImages[PathIndex]);
//             UnloadImage(PreviewImages[PathIndex]);
//         }
//     return;
// }

// internal
// void DrawThumnails(Texture PreviewTextures[], FilePathList PathList)
// {
//     for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
//         {   
//             Texture *texture = PreviewTextures + PathIndex;
//             DrawTexturePro(*texture, (Rectangle){0,0,128,128},(Rectangle){0 + 110.f*PathIndex,0,100,100},(Vector2){0,0},0,WHITE);
//         }
// }


int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AnnRay");
#if TESTTHUMB
    const char *FolderPath = TEST_FOLDER_PNG;
    FilePathList PathList = LoadDirectoryFiles(FolderPath);
    Image PreviewImages[PathList.count];
    Texture PreviewTextures[PathList.count];
    GenerateThumbnails(PreviewImages, PreviewTextures, PathList);
#endif

    while(!WindowShouldClose())
    {   
        AnnotationPage();

// #if TESTTHUMB
//             DrawThumnails(PreviewTextures,PathList);
// #endif

    }

    CloseWindow();
}