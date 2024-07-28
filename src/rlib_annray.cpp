#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <dispatch/dispatch.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

#define PANELWIDTH (f32)280
#define IMAGEDISPLAYSIDEGAP (f32)32

#define MAX_STRINGS 16
#define MAX_LENGTH 16
#define MAX_PROJECT_NAME 16

#include "rlib_annray.h"
#include "annray_math.h"

global_state GlobalState = {.CurrentPage = FRONT_PAGE, .PreviousPage = FRONT_PAGE, .IsProjectSelected = false, .ProjectName = "NoProjectName"};
global u8 CurrentCursorSprite = 0;
global FilePathList PathList;
global Shader shaders = { 0 };
global s32 FragBoxLoc; 
#include "annray_async_image.cpp"
#include "annray_fileio.cpp"
#include "rlib_UIcommons.cpp"
#include "rlib_front_page.cpp"
#include "rlib_new_project_page.cpp"
#include "rlib_load_project_page.cpp"
#include "rlib_annotation_page.cpp"
#include "rlib_inventory_page.cpp"
#include "rlib_export_page.cpp"

    // NOTE: Defining 0 (NULL) for vertex shader forces usage of internal default vertex shader
int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AnnRay");
    SetWindowMinSize(PANELWIDTH + 50, PANELWIDTH + 50);
    SetTargetFPS(120);
    InitAudioDevice();
    shaders = LoadShader(0, TextFormat("../shaders/annotation_focus.fs", 330));
    FragBoxLoc = GetShaderLocation(shaders, "bbox");

    // Start Thread for async loading of image on annotation page
    pthread_t AnnThreadId;
    thread_info_image AnnThreadInfo;
    dispatch_semaphore_t semaphore;
    semaphore = dispatch_semaphore_create(0);
    AnnThreadInfo.SemaphoreHandle = semaphore;
    AnnThreadInfo.AsyncImage.data = nullptr;
    u32 AnnThreadError = pthread_create(&AnnThreadId, NULL,AsyncImageLoading, &AnnThreadInfo);
    if (AnnThreadError != 0) printf("Error creating thread for async image loading on annotation page\n");
    
    Music FrontPageMusic = LoadMusicStream("../assets/awesomeness.wav");
    PlayMusicStream(FrontPageMusic);

    bool IsEnv = IsEnvCreated();
    if (IsEnv == false) CreateEnv();

    // Get last project
    {
        char TmpProjectName[MAX_PROJECT_NAME];
        TextCopy(TmpProjectName, LoadFileText("../projects/last_project.txt"));
        const char *ProjectPath = TextFormat("../projects/%s", TmpProjectName);
        if (DirectoryExists(ProjectPath)) TextCopy(GlobalState.ProjectName, TmpProjectName);
        else TextCopy(GlobalState.ProjectName, "NoProjectName");
        if (!TextIsEqual(GlobalState.ProjectName, "NoProjectName"))
        {
            GlobalState.IsProjectSelected = true;
            PathList = LoadDirectoryFiles(TextFormat("../projects/%s/images", GlobalState.ProjectName));
        } 
         
    }

    bool running = true;
    while(running)
    {   
        running = !WindowShouldClose();
        switch (GlobalState.CurrentPage)
        {

        // The front page dispatch to other pages and also render the page selection page
        case FRONT_PAGE:
        {
            GlobalState.CurrentPage = FrontPage(&FrontPageMusic);
            GlobalState.PreviousPage = FRONT_PAGE;
        break;
        };
        case NEW_PROJECT_PAGE:
        {
             GlobalState.CurrentPage = NewProjectPage();
             GlobalState.PreviousPage = NEW_PROJECT_PAGE;
        break;
        };
        case LOAD_PROJECT_PAGE:
        {
             GlobalState.CurrentPage = LoadProjectPage();
             GlobalState.PreviousPage = LOAD_PROJECT_PAGE;
        break;
        };
        case ANNOTATION_PAGE:
        {
            GlobalState.CurrentPage = AnnotationPage(PathList, &AnnThreadInfo);
            GlobalState.PreviousPage = ANNOTATION_PAGE;
        break;
        };
        case INVENTORY_PAGE:
        {
            GlobalState.CurrentPage = InventoryPage(PathList);
            GlobalState.PreviousPage = INVENTORY_PAGE;

        break;
        }
        case EXPORT_PAGE:
        {
            GlobalState.CurrentPage = ExportPage();
            GlobalState.PreviousPage = EXPORT_PAGE;

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
