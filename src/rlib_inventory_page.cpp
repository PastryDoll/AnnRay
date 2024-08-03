bool ShouldInitInventory = true;

#define MAX_THREADS 8

typedef struct {
    Image *previewImages;
    FilePathList pathList;
    u32 start;
    u32 end;
} ThreadArgs;

void *LoadImagesThread(void *args) {
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

        pthread_create(&threads[i], NULL, LoadImagesThread, (void *)&threadArgs[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex) {
        PreviewTextures[PathIndex] = LoadTextureFromImage(PreviewImages[PathIndex]);
        UnloadImage(PreviewImages[PathIndex]);
    }
}

//@TODO - Ideally it will use the aspect ratio of each pic.. but for now just to have something lets make a simples grid were all pics are squares
internal
void DrawThumnails(Texture PreviewTextures[], FilePathList PathList)
{
    f32 internal ScrollBarY = 0;
    f32 w = 220;
    f32 h = 220;
    f32 MinGapX = 10;
    f32 MinGapY = 10;
    f32 EffectiveW = w + MinGapX; 
    f32 EffectiveH = h + 2*MinGapY;
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    f32 yPad = ScreenHeight*0.01f;
    f32 xPad = ScreenWidth*0.005f;
    f32 GridRowWidth = ScreenWidth - PANELWIDTH - xPad*2;
    // f32 GridRowHeight = ScreenHeight - yPad;
    u32 ImagesPerRow = (u32)((GridRowWidth - EffectiveW)/EffectiveW) + 1;
    // u32 ImagesPerColumn = (u32)(GridRowHeight/EffectiveH);
    f32 ScrollToY = ((u32)(PathList.count-1)/ImagesPerRow*EffectiveH - (ScreenHeight - EffectiveH))/ScreenHeight;

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
            } 
            else
            {
                UnusedX = GridRowWidth - ImagesPerRow*EffectiveW;
                GapX = UnusedX/(ImagesPerRow-1);
                x = xPad + PANELWIDTH + PathIndex%ImagesPerRow*(EffectiveW + GapX);
            }
            y = (u32)PathIndex/ImagesPerRow * EffectiveH + yPad - ScrollBarY*ScrollToY;
            Texture *texture = PreviewTextures + PathIndex;
            DrawTexturePro(*texture, (Rectangle){0,0,(f32)texture->width,(f32)texture->height},(Rectangle){x,y,w,h},(Vector2){0,0},0,WHITE);
        }

    f32 Wheel = GetMouseWheelMove();
    if (Wheel != 0)
    {
        ScrollBarY -= Wheel;
        if (ScrollBarY < 0) ScrollBarY = 0;
        if (ScrollBarY > ScreenHeight) ScrollBarY = ScreenHeight;
    }
    DrawRectangleRounded((Rectangle){ScreenWidth - MinGapX, ScrollBarY-10, MinGapX, 100}, 10, 10, RED); //@TODO - Make opacity fade in-out
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