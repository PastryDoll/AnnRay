bool ShouldInitInventory = true;

#include <pthread.h>

#define MAX_THREADS 8

typedef struct {
    Image *previewImages;
    FilePathList pathList;
    u32 start;
    u32 end;
} ThreadArgs;

void *loadImagesThread(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;

    for (u32 PathIndex = threadArgs->start; PathIndex < threadArgs->end; ++PathIndex) {
        char *Path = *(threadArgs->pathList.paths + PathIndex);
        threadArgs->previewImages[PathIndex] = LoadImage(Path);
        ImageResize(&threadArgs->previewImages[PathIndex], 512, 512);
    }

    pthread_exit(NULL);
}

void GenerateThumbnails(Image PreviewImages[], Texture PreviewTextures[], FilePathList PathList) {
    pthread_t threads[MAX_THREADS];
    ThreadArgs threadArgs[MAX_THREADS];
    int numThreads = PathList.count < MAX_THREADS ? PathList.count : MAX_THREADS;
    int imagesPerThread = PathList.count / numThreads;
    int remainder = PathList.count % numThreads;
    int index = 0;

    for (int i = 0; i < numThreads; ++i) {
        threadArgs[i].previewImages = PreviewImages;
        threadArgs[i].pathList = PathList;
        threadArgs[i].start = index;
        index += imagesPerThread + (i < remainder ? 1 : 0);
        threadArgs[i].end = index;

        pthread_create(&threads[i], NULL, loadImagesThread, (void *)&threadArgs[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex) {
        PreviewTextures[PathIndex] = LoadTextureFromImage(PreviewImages[PathIndex]);
        UnloadImage(PreviewImages[PathIndex]);
    }
}
//@SpeedUp We have to find a nice way to do this.. bc it can be very slow.. we might parallelize and use a finite window
// If not enough we can resize images and save
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

//@TODO - Ideally it will use the aspect ratio of each pic.. but for now just to have something lets make a simples grid were all pics are squares
internal
void DrawThumnails(Texture PreviewTextures[], FilePathList PathList)
{
    f32 w = 200;
    f32 h = 200;
    f32 MinGapX = 10;
    f32 MinGapY = 10;
    f32 EffectiveW = w + MinGapX; 
    f32 EffectiveH = h + 2*MinGapY;
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    f32 yPad = ScreenHeight*0.05f;
    f32 xPad = ScreenWidth*0.005f;
    f32 GridRowWidth = ScreenWidth - PANELWIDTH - xPad*2;
    f32 GridRowHeight = ScreenHeight - yPad;
    u32 ImagesPerRow = (u32)((GridRowWidth - EffectiveW)/EffectiveW) + 1;
    u32 ImagesPerColumn = (u32)(GridRowHeight/EffectiveH);
    printf("ImagesPerRow %u\n",ImagesPerRow);

    f32 UnusedX;
    f32 GapX;
    f32 x;
    f32 y;
    for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
        {   
            if (ImagesPerRow == 1)
            {   
                f32 cx = PANELWIDTH + xPad + GridRowWidth/2 - w/2;
                x = cx > PANELWIDTH ? cx : PANELWIDTH;  
                printf("X: %f\n", x);
                printf("PANELWIDTH: %f\n", PANELWIDTH);
                printf("GridRowWidth/2: %f\n", GridRowWidth);
                printf("xPad: %f\n", xPad);
                printf("w/2: %f\n", w/2);
                printf("PANELWIDTH + xPad + GridRowWidth/2 - w/2: %f\n", PANELWIDTH + xPad + GridRowWidth/2 - w/2);
            } 
            else
            {
                UnusedX = GridRowWidth - ImagesPerRow*EffectiveW;
                GapX = UnusedX/(ImagesPerRow-1);
                x = xPad + PANELWIDTH + PathIndex%ImagesPerRow*(EffectiveW + GapX);
                printf("X: %f\n", x);
                printf("Y: %f\n", y);
                printf("UnusedX: %f\n", UnusedX);
            }
            y = (int)PathIndex/ImagesPerRow * EffectiveH + yPad;
            Texture *texture = PreviewTextures + PathIndex;
            DrawTexturePro(*texture, (Rectangle){0,0,(f32)texture->width,(f32)texture->height},(Rectangle){x,y,w,h},(Vector2){0,0},0,WHITE);

        }
}

internal
void InitializeInventoryPage(Image PreviewImages[], Texture PreviewTextures[], FilePathList PathList)
{
    if(ShouldInitInventory)
    {
        GenerateThumbnails(PreviewImages, PreviewTextures, PathList);
    }
};

internal 
void DrawLeftPanel()
{
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    DrawRectangle(0,0, PANELWIDTH, ScreenHeight, BLACK);
}

internal
u32 InventoryPage(FilePathList PathList)
{
    if (GlobalState.PreviousPage != INVENTORY_PAGE) ShouldInitInventory = true;
    else ShouldInitInventory = false;

    Image PreviewImages[PathList.count];
    Texture PreviewTextures[PathList.count];
    InitializeInventoryPage(PreviewImages, PreviewTextures, PathList);
    BeginDrawing();
        ClearBackground(BLUE);
        DrawLeftPanel();

        {
            if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        }
        DrawThumnails(PreviewTextures,PathList);
        DrawFPS(10,10);
    EndDrawing();

    return INVENTORY_PAGE;
};