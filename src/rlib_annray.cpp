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

global_state GlobalState = {.CurrentPage = FRONT_PAGE, .PreviousPage = FRONT_PAGE};

#include "annray_math.h"
#include "rlib_front_page.cpp"
#include "rlib_annotation_page.cpp"
#include "rlib_inventory_page.cpp"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AnnRay");
    InitAudioDevice();

    const char *FolderPath = TEST_FOLDER_JPG;
    FilePathList PathList = LoadDirectoryFiles(FolderPath);
    Music FrontPageMusic = LoadMusicStream("../assets/awesomeness.wav");
    PlayMusicStream(FrontPageMusic);

    bool running = true;
    while(running)
    {   
        running = !WindowShouldClose();
        switch (GlobalState.CurrentPage)
        {
        case FRONT_PAGE:
        {
            GlobalState.CurrentPage = FrontPage(&FrontPageMusic);
            GlobalState.PreviousPage = FRONT_PAGE;
        break;
        };
        case ANNOTATION_PAGE:
        {
             GlobalState.CurrentPage = AnnotationPage(PathList);
             GlobalState.PreviousPage = ANNOTATION_PAGE;
        break;
        };
        case INVENTORY_PAGE:
        {
            GlobalState.CurrentPage = InventoryPage(PathList);
            GlobalState.PreviousPage = INVENTORY_PAGE;

        break;
        }
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