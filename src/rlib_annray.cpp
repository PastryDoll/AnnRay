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
#define TEST_FOLDER_JPG "../test_material/samples2"
#define PROJECT_FOLDER "../project"
#define TEST_PIC "DJI_0325.JPG"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

#define PANELWIDTH (float)280
#define IMAGEDISPLAYSIDEGAP (float)32

#include "annray_math.h"
#include "rlib_front_page.cpp"
#include "rlib_annotation_page.cpp"
#include "rlib_inventory_page.cpp"

#define TESTTHUMB 0

global_state GlobalState = {.CurrentPage = FRONT_PAGE};

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AnnRay");
    InitAudioDevice();

    const char *FolderPath = TEST_FOLDER_JPG;
    FilePathList PathList = LoadDirectoryFiles(FolderPath);
    Music FrontPageMusic = LoadMusicStream("../assets/awesomeness.wav");
    PlayMusicStream(FrontPageMusic);

#if TESTTHUMB
    Image PreviewImages[PathList.count];
    Texture PreviewTextures[PathList.count];
    GenerateThumbnails(PreviewImages, PreviewTextures, PathList);
#endif
    bool running = true;
    while(running)
    {   
        running = !WindowShouldClose();
        switch (GlobalState.CurrentPage)
        {
        case FRONT_PAGE:
        {
            GlobalState.CurrentPage = FrontPage(&FrontPageMusic);
        break;
        };
        case ANNOTATION_PAGE:
        {
            AnnotationPage(PathList);
            break;
        };
        case EXIT:
        {
            StopMusicStream(FrontPageMusic);
            running = false;
        }
        default: {};
        };
    }
    CloseWindow();
}