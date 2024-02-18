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

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

#define IMAGEDISPLAY_WIDTH 1080
#define IMAGEDISPLAY_HEIGHT 720

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

    const char *ImagePath = TextFormat("%s/%s",TEST_FOLDER_JPG, TEST_PIC);
    Texture CurrentTexture = LoadTexture(ImagePath);
    
    s32 count = 0;
    const char *ImageName = TextSplit(TEST_PIC,'.',&count)[0];
    const char *AnnPathTmp = TextFormat("%s/%s.ann",PROJECT_FOLDER, ImageName);
    char* AnnPath = (char*)malloc(strlen(AnnPathTmp) + 1);
    strcpy(AnnPath,AnnPathTmp);

    disp_mode DisplayMode = DispMode_creation;

    f32 TextureRatio = (float)CurrentTexture.height/CurrentTexture.width;



    // SetTargetFPS(600);   


#if TESTTHUMB
    const char *FolderPath = TEST_FOLDER_PNG;
    FilePathList PathList = LoadDirectoryFiles(FolderPath);
    Image PreviewImages[PathList.count];
    Texture PreviewTextures[PathList.count];
    GenerateThumbnails(PreviewImages, PreviewTextures, PathList);
#endif
    zoom Zoom;
    Zoom.Strenght = 1.0f;
    Zoom.Position = {0.0f,0.0f};
    // bbox Bboxes[10] = {};
    // u32 CurrentLabel = 0;
    // u32 CurrentBbox = 0;
    // u32 TotalBbox = 0;
    render_display RenderDisplay = {};

    while(!WindowShouldClose())
    {   
        Vector2 MousePosition = GetMousePosition();
        u32 CurrentGesture = GetGestureDetected();

        u32 ScreenWidth = GetScreenWidth();
        u32 ScreenHeight = GetScreenHeight();

        f32 dt = GetFrameTime();

        AnnotationPage(&RenderDisplay);
        // BeginDrawing();

            // ClearBackground(GRAY);

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

// // Inside Lateral Menu Stuff

//             DrawRectangle(0,0,PANELWIDTH,ScreenHeight,BLACK);

//             internal s32 Active = {};
//             GuiToggleGroup((Rectangle){0,120,150,20},TextJoin(Labels,ArrayCount(Labels),"\n"),&Active);
//             CurrentLabel = Active;

//             for (u32 LabelId = 0; LabelId < ArrayCount(Labels);  ++LabelId)
//             {   
//                 DrawRectangle(140,120 + 22*LabelId,10,20,LabelsColors[LabelId]);
//             }

//             if (IsKeyReleased(KEY_ONE))
//             {
//                 CurrentLabel = 0;
//                 Active = 0;
//             }
//             else if (IsKeyReleased(KEY_TWO))
//             {
//                 CurrentLabel = 1;
//                 Active = 1;

//             }
//             else if (IsKeyReleased(KEY_THREE))
//             {
//                 CurrentLabel = 2;
//                 Active = 2;

//             }
//             else if (IsKeyReleased(KEY_FOUR))
//             {
//                 CurrentLabel = 3;
//                 Active = 3;

//             }
//             else if (IsKeyReleased(KEY_FIVE))
//             {
//                 CurrentLabel = 4;
//                 Active = 4;

//             }
//             else if (IsKeyReleased(KEY_SIX))
//             {
//                 CurrentLabel = 5;
//                 Active = 5;
//             }

//             if (CheckCollisionPointRec(MousePosition,(Rectangle){0,0,PANELWIDTH,(float)ScreenHeight}))
//             {
//                 SetMouseCursor(MOUSE_CURSOR_DEFAULT);
//             }
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

// // Inside Image Display Stuff
//             f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
//             f32 FullImageDisplayHeight = ScreenHeight;

//             f32 ImageDisplayWidth = FullImageDisplayWidth - IMAGEDISPLAYSIDEGAP;
//             f32 ImageDisplayHeight = ImageDisplayWidth*TextureRatio;

//             Rectangle ImageDisplayRec;
//             {
//                 f32 w = (float)CurrentTexture.width;
//                 f32 h = (float)CurrentTexture.height;

//                 Rectangle TextureRec = {Zoom.Position.x*w,Zoom.Position.y*h,w/Zoom.Strenght,h/Zoom.Strenght};
//                 ImageDisplayRec = {PANELWIDTH + IMAGEDISPLAYSIDEGAP/2,FullImageDisplayHeight/2 - ImageDisplayHeight/2,ImageDisplayWidth,ImageDisplayHeight};
//                 DrawTexturePro(CurrentTexture,TextureRec,ImageDisplayRec,(Vector2){0,0},0,WHITE);\
//             }

            
//             if (IsKeyDown(KEY_W))
//             {
//                 Zoom.Strenght += 0.9*dt;
//             }
//             if (IsKeyDown(KEY_S))
//             {
//                 Zoom.Strenght -= 0.9*dt;
//             }
//             if (IsKeyDown(KEY_LEFT))
//             {
//                 Zoom.Position.x -= 0.5*dt;
//             }
//             if (IsKeyDown(KEY_RIGHT))
//             {
//                 Zoom.Position.x += 0.5*dt;
//             }
//             if (IsKeyDown(KEY_UP))
//             {
//                 Zoom.Position.y -= 0.5*dt;
//             }
//             if (IsKeyDown(KEY_DOWN))
//             {
//                 Zoom.Position.y += 0.5*dt;
//             }
//             if (IsKeyPressed(KEY_D))
//             {
//                 DisplayMode = DispMode_manipulation;
//             }
//             else if (IsKeyPressed(KEY_B))
//             {
//                 DisplayMode = DispMode_creation;
//             }

//             if (CheckCollisionPointRec(MousePosition,(Rectangle){PANELWIDTH,0,FullImageDisplayWidth,FullImageDisplayHeight}))
//             {
//                 DrawSegmentedLines(MousePosition.x,MousePosition.y,ScreenWidth,ScreenHeight,LabelsColors[CurrentLabel]);

//                 switch (DisplayMode)
//                 {
//                     case DispMode_creation:
//                     {
//                         // Maybe this function is a bad idea and we should inline it
//                         TotalBbox = BoxCreation(TotalBbox,&CurrentBbox,CurrentGesture,CurrentLabel,MousePosition,Bboxes,AnnPath);
//                     break;
//                     }
//                     case DispMode_manipulation:
//                     {
//                         BoxManipulation(TotalBbox, CurrentGesture, MousePosition, Bboxes);
//                     break;
//                     }
//                 }
//             }
//             BeginScissorMode(ImageDisplayRec.x, ImageDisplayRec.y,ImageDisplayRec.width,ImageDisplayRec.height);
//                 for (u32 BoxId = 0; BoxId < TotalBbox + 1; ++BoxId)
//                 {
//                     DrawRectangleLinesEx(Bboxes[BoxId].Box,2,LabelsColors[Bboxes[BoxId].Label]);
//                 }
//                 {
//                     u32 x = (f32)Bboxes[CurrentBbox].Box.x;
//                     u32 y = (f32)Bboxes[CurrentBbox].Box.y;
//                     u32 w = (f32)Bboxes[CurrentBbox].Box.width;
//                     u32 h = (f32)Bboxes[CurrentBbox].Box.height;
//                     DrawRectangle(x - 2,y - 2,6,6,RAYWHITE);
//                     DrawRectangle(x + w - 3,y - 2,6,6,RAYWHITE);
//                     DrawRectangle(x + w - 3,y + h - 3,6,6,RAYWHITE);
//                     DrawRectangle(x - 1,y + h - 3,6,6,RAYWHITE);
//                 }
//             EndScissorMode();
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  


// #if TESTTHUMB
//             DrawThumnails(PreviewTextures,PathList);
// #endif

//             DrawFPS(10,10);
//         EndDrawing();
    }

    CloseWindow();
}