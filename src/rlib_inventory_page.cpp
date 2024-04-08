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
        ImageResize(&threadArgs->previewImages[PathIndex], 128, 128);
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

internal
void DrawThumnails(Texture PreviewTextures[], FilePathList PathList)
{
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();

    f32 y = 0.0f;
    for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
        {   
            f32 x = ScreenWidth*0.4 + (PathIndex % 4)*110.f;
            y += ((PathIndex + 1) % 4 == 0) * (128 + 20);
            Texture *texture = PreviewTextures + PathIndex;
            DrawTexturePro(*texture, (Rectangle){0,0,128,128},(Rectangle){x,y,100,100},(Vector2){0,0},0,WHITE);
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

// #if TESTTHUMB
// #endif

internal
void InventoryPage(FilePathList PathList)
{
    if (GlobalState.PreviousPage != INVENTORY_PAGE) ShouldInitInventory = true;
    else ShouldInitInventory = false;


    Image PreviewImages[PathList.count];
    Texture PreviewTextures[PathList.count];
    InitializeInventoryPage(PreviewImages, PreviewTextures, PathList);
    BeginDrawing();
        ClearBackground(BLUE);
        DrawThumnails(PreviewTextures,PathList);
        DrawFPS(10,10);
    EndDrawing();


};