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

#define TEST_FOLDER_PNG "../images_samplepng/"
#define TEST_FOLDER_JPG "../test_material/images_sample"
#define PROJECT_FOLDER "../project"
#define TEST_PIC "DJI_0325.JPG"

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

internal const
void SaveDataToFile(const char *FileName, bbox *Boxes, u32 NumBoxes)
{
    FILE *file = fopen(FileName, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    printf("%s\n",FileName);

    fwrite(Boxes, sizeof(BoundingBox), NumBoxes, file);

    fclose(file);
}

internal 
void ReadInMemoryAnn(const char *FileName, u32 NumBoxes)
{
    FILE *file;
    file = fopen(FileName, "rb");
    if (file == NULL) {
        perror("Error opening file");
    }
    bbox Boxes[NumBoxes];
    fread(Boxes, sizeof(bbox), NumBoxes, file);

    for (u32 BoxId = 0; BoxId < NumBoxes; ++BoxId)
    {  
        printf("id: %u,x: %f,y: %f,w: %f,h: %f,label: %u\n", BoxId,Boxes[BoxId].Box.x,Boxes[BoxId].Box.y,Boxes[BoxId].Box.width,Boxes[BoxId].Box.height, Boxes[BoxId].Label);
    }
    fclose(file);
}

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

internal const
void DrawSegmentedLines(f32 X, f32 Y, f32 W, f32 H, Color color)
{   
    const u32 gap = 4;
    const f32 SegLen = 12;

    f32 LenX = W - PANELWIDTH;
    u32 NSegX = (u32)ceil(LenX/SegLen + gap);

    for (u32 Seg = 0; Seg < NSegX; ++Seg)
    {
        f32 start = PANELWIDTH + Seg*(SegLen + gap) - SegLen/2;
        f32 end = start + SegLen;
        DrawLineEx({start,Y},{end,Y},2,color);
    }

    f32 LenY = H - 0;
    u32 NSegY = (u32)ceil(LenY/SegLen + gap);

    for (u32 Seg = 0; Seg < NSegY; ++Seg)
    {
        f32 start = 0 + Seg*(SegLen + gap) - SegLen/2;
        f32 end = start + SegLen;
        DrawLineEx({X,start},{X,end},2,color);
    }
}

internal inline
u32 BoxManipulation(u32 Total, u32 CurrentGesture, u32 CurrentLabel, Vector2 MousePosition, bbox Bboxes[], const char* AnnPath)
{
    internal u32 PrevGesture = 3;
    internal u32 CurrentBbox = 0;
    Rectangle *BBox = &Bboxes[CurrentBbox].Box;
    Bboxes[CurrentBbox].Label = CurrentLabel;
    internal Vector2 Tap = {};

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
    // If one goes too fast than the prevGesture can be also swipedown, not juts drag or hold.
    if (BBox->width > 0.1 && ~(PrevGesture & (GESTURE_NONE)) && (CurrentGesture == (GESTURE_NONE)) && Total <= 10)
    {
        CurrentBbox += 1;
        Total += 1;
        SaveDataToFile(AnnPath,Bboxes,Total);
        ReadInMemoryAnn(AnnPath, Total);
    }
    PrevGesture = CurrentGesture;

    return(Total);
}

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

    f32 TextureRatio = (float)CurrentTexture.height/CurrentTexture.width;
    SetTargetFPS(600);   


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
    u32 CurrentLabel = 0;
    u32 TotalBbox = 0;

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

            DrawRectangle(0,0,PANELWIDTH,ScreenHeight,BLACK);

            internal s32 Active = {};
            GuiToggleGroup((Rectangle){0,120,150,20},TextJoin(Labels,ArrayCount(Labels),"\n"),&Active);
            CurrentLabel = Active;

            for (u32 LabelId = 0; LabelId < ArrayCount(Labels);  ++LabelId)
            {   
                DrawRectangle(140,120 + 22*LabelId,10,20,LabelsColors[LabelId]);
            }

            if (IsKeyReleased(KEY_ONE))
            {
                CurrentLabel = 0;
                Active = 0;
            }
            else if (IsKeyReleased(KEY_TWO))
            {
                CurrentLabel = 1;
                Active = 1;

            }
            else if (IsKeyReleased(KEY_THREE))
            {
                CurrentLabel = 2;
                Active = 2;

            }
            else if (IsKeyReleased(KEY_FOUR))
            {
                CurrentLabel = 3;
                Active = 3;

            }
            else if (IsKeyReleased(KEY_FIVE))
            {
                CurrentLabel = 4;
                Active = 4;

            }
            else if (IsKeyReleased(KEY_SIX))
            {
                CurrentLabel = 5;
                Active = 5;
            }

            if (CheckCollisionPointRec(MousePosition,(Rectangle){0,0,PANELWIDTH,(float)ScreenHeight}))
            {
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
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
                SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
                DrawSegmentedLines(MousePosition.x,MousePosition.y,ScreenWidth,ScreenHeight,LabelsColors[CurrentLabel]);
                // Maybe this function is a bad idea and we should inline it
                TotalBbox = BoxManipulation(TotalBbox,CurrentGesture,CurrentLabel,MousePosition,Bboxes,AnnPath);
                
            }

            BeginScissorMode(ImageDisplayRec.x, ImageDisplayRec.y,ImageDisplayRec.width,ImageDisplayRec.height);
            for (u32 BoxId = 0; BoxId < TotalBbox + 1; ++BoxId)
            {
                DrawRectangleLinesEx(Bboxes[BoxId].Box,2,LabelsColors[Bboxes[BoxId].Label]);
            }
            EndScissorMode();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  


#if TESTTHUMB
            DrawThumnails(PreviewTextures,PathList);
#endif

            DrawFPS(10,10);
        EndDrawing();
    }

    CloseWindow();
}