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

#include "rlib_annray.h"
#include "annray_math.h"

#define TEST_FOLDER_PNG "/Users/caio/Desktop/Personal/CppTests/AnnRay/images_samplepng/"
#define TEST_FOLDER_JPG "/Users/caio/Desktop/Personal/CppTests/AnnRay/test_material/images_sample"
#define TEST_PIC "/Users/caio/Desktop/Personal/CppTests/AnnRay/test_material/images_sample/DJI_0325.JPG"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

#define PANELWIDTH (float)280
#define IMAGEDISPLAYSIDEGAP (float)32

#define TESTTHUMB 0

const global Color LabelsColors[10] = {RED,WHITE,GREEN,BLUE,MAGENTA,YELLOW,PURPLE,BROWN,SKYBLUE,LIME};
const global char *Labels[] = {"BOX", "STICKER", "COW", "DOG", "PNEUMOTORAX"};

struct bbox
{
    u32 Label;
    Rectangle Box;
};

struct zoom
{
    f32 Strenght;
    Vector2 Position;
};

internal
void GenerateThumbnails(Image PreviewImages[], Texture PreviewTextures[], FilePathList PathList)
{   
    for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
        {
            char *Path = *(PathList.paths + PathIndex);
            PreviewImages[PathIndex] = LoadImage(Path);
            ImageResize(&PreviewImages[PathIndex],128,128);
            PreviewTextures[PathIndex] = LoadTextureFromImage(PreviewImages[PathIndex]);
            UnloadImage(PreviewImages[PathIndex]);
        }
    return;
}

internal
void DrawThumnails(Texture PreviewTextures[], FilePathList PathList)
{
    for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
        {   
            Texture *texture = PreviewTextures + PathIndex;
            DrawTexturePro(*texture, (Rectangle){0,0,128,128},(Rectangle){0 + 110.f*PathIndex,0,100,100},(Vector2){0,0},0,WHITE);
        }
}

internal inline
u32 BoxManipulation(u32 Total, u32 CurrentGesture, u32 CurrentLabel, Vector2 MousePosition, bbox Bboxes[])
{
    internal u32 PrevGesture = 3;
    internal u32 CurrentBbox = 0;
    Rectangle *BBox = &Bboxes[CurrentBbox].Box;
    Bboxes[CurrentBbox].Label = CurrentLabel;
    internal Vector2 Tap;

    if (CurrentGesture & (GESTURE_TAP))
    {
        Tap.x = MousePosition.x;
        Tap.y = MousePosition.y;
    }
    else
    {
        if (CurrentGesture & (GESTURE_DRAG|GESTURE_HOLD))
        {
            // Down-Right
            if ((MousePosition.x >= Tap.x) && (MousePosition.y >= Tap.y))
            {   
                BBox->x = Tap.x;
                BBox->y = Tap.y;
                BBox->width = MousePosition.x - Tap.x;
                BBox->height = MousePosition.y - Tap.y;
            }
            // Upper Right
            else if ((MousePosition.x > Tap.x) && (MousePosition.y < Tap.y))
            {
                BBox->width = MousePosition.x - Tap.x;
                BBox->height = Tap.y - MousePosition.y;
                BBox->x = Tap.x;
                BBox->y = MousePosition.y;
            }
            // Down Left
            else if ((MousePosition.x < Tap.x) && (MousePosition.y > Tap.y))
            {
                BBox->width = Tap.x - MousePosition.x;
                BBox->height = MousePosition.y - Tap.y;
                BBox->x = MousePosition.x;
                BBox->y = Tap.y;
            }
            // Upper Left
            else if ((MousePosition.x < Tap.x) && (MousePosition.y < Tap.y))
            {
                BBox->width = Tap.x - MousePosition.x;
                BBox->height = Tap.y - MousePosition.y;
                BBox->x = MousePosition.x;
                BBox->y = MousePosition.y;
            }
        }
    }
    // printf("BBox: %f\n",BBox->width);
    // printf("BBoxarray: %f\n",Bboxes[CurrentBbox].width);
    if ((PrevGesture & (GESTURE_DRAG|GESTURE_HOLD)) && (CurrentGesture == (GESTURE_NONE)))
    {
        CurrentBbox += 1;
        Total += 1;
    }
    PrevGesture = CurrentGesture;

    return(Total);
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AnnRay");

    SetTargetFPS(240);   
    const char *FolderPath = TEST_PIC;
    Texture CurrentTexture = LoadTexture(FolderPath);
    f32 TextureRatio = (float)CurrentTexture.height/CurrentTexture.width;

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
    bbox Bboxes[10] = {};
    u32 TotalBbox = 1;
    u32 CurrentLabel = 0;

    while(!WindowShouldClose())
    {   
        Vector2 MousePosition = GetMousePosition();
        u32 CurrentGesture = GetGestureDetected();

        u32 ScreenWidth = GetScreenWidth();
        u32 ScreenHeight = GetScreenHeight();

        f32 dt = GetFrameTime();

        BeginDrawing();

            ClearBackground(GRAY);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

// Inside Lateral Menu Stuff
            if (IsKeyReleased(KEY_ONE))
            {
                CurrentLabel = 0;
            }
            else if (IsKeyReleased(KEY_TWO))
            {
                CurrentLabel = 1;
            }
            else if (IsKeyReleased(KEY_THREE))
            {
                CurrentLabel = 2;
            }
            else if (IsKeyReleased(KEY_FOUR))
            {
                CurrentLabel = 3;
            }
            else if (IsKeyReleased(KEY_FIVE))
            {
                CurrentLabel = 4;
            }
            else if (IsKeyReleased(KEY_SIX))
            {
                CurrentLabel = 5;
            }

            DrawRectangle(0,0,PANELWIDTH,ScreenHeight,BLACK);
            // int active[ArrayCount(Labels)];
            int active;
            
            for (u32 LabelId = 0; LabelId < ArrayCount(Labels);  ++LabelId)
            {   
                u32 StrWidth = MeasureText(Labels[LabelId],10) + 20;
                GuiToggleGroup((Rectangle){0,120.f + 15.f*LabelId,(float)StrWidth,10},"",&active);
                // GuiToggleGroup((Rectangle){0,120.f + 15.f*LabelId,(float)StrWidth,10},"",&active[LabelId]);
                // GuiToggle((Rectangle){0,120.f + 15.f*LabelId,(float)StrWidth,10},"",&active[LabelId]);
                DrawText(Labels[LabelId], 10, 120 + 15*LabelId, 10, BLACK);
                DrawRectangle(StrWidth,120 + 15*LabelId,10,10,LabelsColors[LabelId]);
            }
            if (CheckCollisionPointRec(MousePosition,(Rectangle){0,0,PANELWIDTH,(float)ScreenHeight}))
            {

            }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

// Inside Image Display Stuff
            f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
            f32 FullImageDisplayHeight = ScreenHeight;

            f32 ImageDisplayWidth = FullImageDisplayWidth - IMAGEDISPLAYSIDEGAP;
            f32 ImageDisplayHeight = ImageDisplayWidth*TextureRatio;

            f32 w = (float)CurrentTexture.width;
            f32 h = (float)CurrentTexture.height;

            Rectangle TextureRec = {Zoom.Position.x*w,Zoom.Position.y*h,w/Zoom.Strenght,h/Zoom.Strenght};
            Rectangle ImageDisplayRec = {PANELWIDTH + IMAGEDISPLAYSIDEGAP/2,FullImageDisplayHeight/2 - ImageDisplayHeight/2,ImageDisplayWidth,ImageDisplayHeight};

            DrawTexturePro(CurrentTexture,TextureRec,ImageDisplayRec,(Vector2){0,0},0,WHITE);
            
            if (IsKeyDown(KEY_W))
            {
                Zoom.Strenght += 0.9*dt;
            }
            if (IsKeyDown(KEY_S))
            {
                Zoom.Strenght -= 0.9*dt;
            }
            if (IsKeyDown(KEY_LEFT))
            {
                Zoom.Position.x -= 0.5*dt;
            }
            if (IsKeyDown(KEY_RIGHT))
            {
                Zoom.Position.x += 0.5*dt;
            }
            if (IsKeyDown(KEY_UP))
            {
                Zoom.Position.y -= 0.5*dt;
            }
            if (IsKeyDown(KEY_DOWN))
            {
                Zoom.Position.y += 0.5*dt;
            }

            if (CheckCollisionPointRec(MousePosition,(Rectangle){PANELWIDTH,0,FullImageDisplayWidth,FullImageDisplayHeight}))
            {
                DrawLineEx((Vector2){MousePosition.x + FullImageDisplayWidth,MousePosition.y}, MousePosition - (Vector2){MousePosition.x - PANELWIDTH,0}, 2, LabelsColors[CurrentLabel]);
                DrawLineEx((Vector2){MousePosition.x,MousePosition.y + FullImageDisplayHeight}, MousePosition - (Vector2){0,MousePosition.y}, 2, LabelsColors[CurrentLabel]);
                // Maybe this function is a bad idea and we should inline it
                TotalBbox = BoxManipulation(TotalBbox,CurrentGesture,CurrentLabel,MousePosition,Bboxes);
                
                BeginScissorMode(ImageDisplayRec.x, ImageDisplayRec.y,ImageDisplayRec.width,ImageDisplayRec.height);
                for (u32 BoxId = 0; BoxId < TotalBbox; ++BoxId)
                {
                    DrawRectangleLinesEx(Bboxes[BoxId].Box,2,LabelsColors[Bboxes[BoxId].Label]);
                }
                EndScissorMode();
            }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  


#if TESTTHUMB
            DrawThumnails(PreviewTextures,PathList);
#endif

            DrawFPS(10,10);
        EndDrawing();
    }

    CloseWindow();
}