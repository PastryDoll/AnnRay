#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>

#include "raylib.h"
#include "rlib_annray.h"
#include "annray_math.h"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

#define TESTTHUMB 0

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

struct zoom
{
    f32 Strenght;
    Vector2 Position;
};

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayRay");

    // SetTargetFPS(60);   
    const char *FolderPath = "/Users/caio/Desktop/Personal/CppTests/AnnRay/images_sample/DJI_0325.JPG";
    Texture CurrentTexture = LoadTexture(FolderPath);
    f32 TextureRatio = (float)CurrentTexture.height/CurrentTexture.width;

#if TESTTHUMB
    const char *FolderPath = "/Users/caio/Desktop/Personal/CppTests/AnnRay/images_samplepng/";
    FilePathList PathList = LoadDirectoryFiles(FolderPath);
    Image PreviewImages[PathList.count];
    Texture PreviewTextures[PathList.count];
    GenerateThumbnails(PreviewImages, PreviewTextures, PathList);
#endif
    zoom Zoom;
    Zoom.Strenght = 1.0f;
    Zoom.Position = {0.0f,0.0f};

    while(!WindowShouldClose())
    {   
        Vector2 MousePosition = GetMousePosition();
        u32 CurrentGesture = GetGestureDetected();

        u32 ScreenWidth = GetScreenWidth();
        u32 ScreenHeight = GetScreenHeight();
         
        f32 PanelWidth = 280;
        f32 ImageDisplaySideGap = 32;

        f32 FullImageDisplayWidth = ScreenWidth > PanelWidth ? ScreenWidth - PanelWidth : 0;
        f32 FullImageDisplayHeight = ScreenHeight;

        f32 ImageDisplayWidth = FullImageDisplayWidth - ImageDisplaySideGap;
        f32 ImageDisplayHeight = ImageDisplayWidth*TextureRatio;

        Rectangle ImageDisplayRec = {PanelWidth + ImageDisplaySideGap/2,(float)ScreenHeight/2 - ImageDisplayHeight/2,ImageDisplayWidth,ImageDisplayHeight};
        

        BeginDrawing();
            ClearBackground(GRAY);

            DrawRectangle(0,0,PanelWidth, ScreenHeight,BLACK);

            if (IsKeyPressed(KEY_W))
            {
                Zoom.Strenght += 0.1;
            }
            if (IsKeyPressed(KEY_S))
            {
                Zoom.Strenght -= 0.1;
            }
            if (IsKeyPressed(KEY_LEFT))
            {
                Zoom.Position.x -= 0.01;
            }
            if (IsKeyPressed(KEY_RIGHT))
            {
                Zoom.Position.x += 0.01;
            }
            if (IsKeyPressed(KEY_UP))
            {
                Zoom.Position.y -= 0.01;
            }
            if (IsKeyPressed(KEY_DOWN))
            {
                Zoom.Position.y += 0.01;
            }
            
            f32 w = (float)CurrentTexture.width;
            f32 h = (float)CurrentTexture.height;
            printf("zoom: %f\n",Zoom.Strenght);
            Rectangle TextureRec = {Zoom.Position.x*w,Zoom.Position.y*h,w/Zoom.Strenght,h/Zoom.Strenght};
            DrawTexturePro(CurrentTexture,TextureRec,ImageDisplayRec,(Vector2){0,0},0,WHITE);
            if (CheckCollisionPointRec(MousePosition,(Rectangle){PanelWidth,0,FullImageDisplayWidth,FullImageDisplayHeight}))
            {
                DrawLineEx((Vector2){MousePosition.x + FullImageDisplayWidth,MousePosition.y}, MousePosition - (Vector2){MousePosition.x - PanelWidth,0}, 2, WHITE);
                DrawLineEx((Vector2){MousePosition.x,MousePosition.y + FullImageDisplayHeight}, MousePosition - (Vector2){0,MousePosition.y}, 2, WHITE);

                Rectangle BBox;
                Vector2 Tap;
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
                            BBox.x = Tap.x;
                            BBox.y = Tap.y;
                            BBox.width = MousePosition.x - Tap.x;
                            BBox.height = MousePosition.y - Tap.y;
                        }
                        // Upper Right
                        else if ((MousePosition.x > Tap.x) && (MousePosition.y < Tap.y))
                        {
                            BBox.width = MousePosition.x - Tap.x;
                            BBox.height = Tap.y - MousePosition.y;
                            BBox.x = Tap.x;
                            BBox.y = MousePosition.y;
                        }
                        // Down Left
                        else if ((MousePosition.x < Tap.x) && (MousePosition.y > Tap.y))
                        {
                            BBox.width = Tap.x - MousePosition.x;
                            BBox.height = MousePosition.y - Tap.y;
                            BBox.x = MousePosition.x;
                            BBox.y = Tap.y;
                        }
                        // Upper Left
                        else if ((MousePosition.x < Tap.x) && (MousePosition.y < Tap.y))
                        {
                            BBox.width = Tap.x - MousePosition.x;
                            BBox.height = Tap.y - MousePosition.y;
                            BBox.x = MousePosition.x;
                            BBox.y = MousePosition.y;
                        }
                        BeginScissorMode(ImageDisplayRec.x, ImageDisplayRec.y,ImageDisplayRec.width,ImageDisplayRec.height);
                        DrawRectangleLinesEx(BBox,2,RED);
                        EndScissorMode();
                    }
                }
            }

#if TESTTHUMB
            DrawThumnails(PreviewTextures,PathList);
#endif

            DrawFPS(10,10);
        EndDrawing();
    }

    CloseWindow();
}