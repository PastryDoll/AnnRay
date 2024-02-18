#include "rlib_annotation_menu.h"

const global Color LabelsColors[10] = {RED,WHITE,GREEN,BLUE,MAGENTA,YELLOW,PURPLE,BROWN,SKYBLUE,LIME};
const global char *Labels[] = {"BOX", "STICKER", "COW", "DOG", "PNEUMOTORAX"};
bbox Bboxes[10] = {};
u32 CurrentLabel = 0;
u32 CurrentBbox = 0;
u32 TotalBbox = 0;

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

internal const
void DrawSegmentedLines(f32 X, f32 Y, f32 W, f32 H, Color color)
{   
    const u32 gap = 4;
    const f32 SegLen = 12;

    f32 LenX = W;
    u32 NSegX = (u32)ceil(LenX/SegLen + gap);

    for (u32 Seg = 0; Seg < NSegX; ++Seg)
    {
        f32 start = Seg*(SegLen + gap) - SegLen/2;
        f32 end = start + SegLen;
        DrawLineEx({start,Y},{end,Y},2,color);
    }

    f32 LenY = H;
    u32 NSegY = (u32)ceil(LenY/SegLen + gap);

    for (u32 Seg = 0; Seg < NSegY; ++Seg)
    {
        f32 start = 0 + Seg*(SegLen + gap) - SegLen/2;
        f32 end = start + SegLen;
        DrawLineEx({X,start},{X,end},2,color);
    }
}

internal inline
void BoxManipulation(u32 Total, u32 CurrentGesture, Vector2 MousePosition, bbox Bboxes[])
{
    box_hit_state CollisionState = NoHit;
    f32 e = 8;
    s32 CollisionId = -1;

    for (u32 BoxId = 0; BoxId < Total; ++BoxId)
    {
        if (CheckCollisionPointRec(MousePosition,Bboxes[BoxId].Box))
        {
            CollisionState = InsideHit;
            CollisionId = BoxId;

            // Right Logic || Left Logic
            if  ((MousePosition.x > (Bboxes[BoxId].Box.x + Bboxes[BoxId].Box.width - e)) 
                || (MousePosition.x < (Bboxes[BoxId].Box.x + e)))
            {
                CollisionState = HorizontalHit;
            }
            // Botton Logic || Top Logic
            else if ((MousePosition.y > (Bboxes[BoxId].Box.y + Bboxes[BoxId].Box.height - e)) 
                || (MousePosition.y < (Bboxes[BoxId].Box.y + e)))
            {
                CollisionState = VerticalHit;
            }
            
            break;
        }
    }

    switch (CollisionState)
    {
        case NoHit:
        {
            SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
        break;
        }
        case InsideHit:
        {   
            internal Vector2 Tap = {};
            if (CurrentGesture & (GESTURE_TAP))
            {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
            }
            else if (CurrentGesture & (GESTURE_DRAG))
            {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
                Bboxes[CollisionId].Box.x += MousePosition.x - Tap.x;
                Bboxes[CollisionId].Box.y += MousePosition.y - Tap.y;
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
            }
            else if (CurrentGesture & (GESTURE_HOLD))
            {
                SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);

            }
            else
            {
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            }

        break;
        }
        case HorizontalHit:
        {
            SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
        break;
        }
        case VerticalHit:
        {
            SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
        break;
        }
    }
}

internal inline
u32 BoxCreation(u32 Total, u32 *CurrentBbox, u32 CurrentLabel, Vector2 MousePosition, bbox Bboxes[])//, const char* AnnPath)
{
    SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    internal u32 PrevGesture = 3;
    Rectangle *BBox = &Bboxes[Total].Box;
    Bboxes[Total].Label = CurrentLabel;
    internal Vector2 Tap = {};
    u32 CurrentGesture = GetGestureDetected();

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
        *CurrentBbox = Total;
        Total += 1;
        // SaveDataToFile(AnnPath,Bboxes,Total);
        // ReadInMemoryAnn(AnnPath, Total);
    }
    PrevGesture = CurrentGesture;

    return(Total);
}

internal
void ImageDisplay(RenderTexture2D ImageDisplayTexture, render_display *RenderDisplay, disp_mode DisplayMode)
{
    f32 RenderWidth = ImageDisplayTexture.texture.width;
    f32 RenderHeight = ImageDisplayTexture.texture.height;
    Vector2 MousePosition = GetMousePosition();
    Vector2 MousePositionRelative = Vector2Clamp(MousePosition, {PANELWIDTH,0.0f}, {PANELWIDTH + RenderWidth, RenderHeight});
    MousePositionRelative.x -= PANELWIDTH;

    // printf("Target: %f,%f\n", RenderDisplay->camera.target.x,RenderDisplay->camera.target.y);
    // printf("offset: %f,%f\n", RenderDisplay->camera.offset.x,RenderDisplay->camera.offset.y);

    u32 gest = GetGestureDetected();
    // if (gest == GESTURE_DRAG)
    // {
    //     Vector2 delta = GetMouseDelta();
    //     delta = Vector2Scale(delta, -1.0f/RenderDisplay->camera.zoom);

    //     RenderDisplay->camera.target = Vector2Add(RenderDisplay->camera.target, delta);
    // }

    f32 wheel = GetMouseWheelMove();
    Vector2 mouseWorldPos = GetScreenToWorld2D(MousePositionRelative, RenderDisplay->camera);
    if (wheel != 0)
    {

        RenderDisplay->camera.offset = MousePositionRelative;
        RenderDisplay->camera.target = mouseWorldPos;

        const f32 zoomIncrement = 0.125f;

        RenderDisplay->camera.zoom += (wheel*zoomIncrement);
        if (RenderDisplay->camera.zoom < zoomIncrement) RenderDisplay->camera.zoom = zoomIncrement;
    }

    switch (DisplayMode)
    {
        case DispMode_creation:
        {
            // Maybe this function is a bad idea and we should inline it
            TotalBbox = BoxCreation(TotalBbox,&CurrentBbox,CurrentLabel,mouseWorldPos,Bboxes);//,AnnPath);
        break;
        }
        case DispMode_manipulation:
        {
            // BoxManipulation(TotalBbox, CurrentGesture, MousePosition, Bboxes);
        break;
        }
    }

    //@NOTE(CAIO) I'm dumb and it took me a long time to understand how the camera 2D works...
    //So we have world coordinates (used in BeginMode2D) and screen coordinates. Target is the 
    //world coordinate for left top of the screen, but if we want we can use offset to move the target
    //to another point on the screen.. Our zoom now works by setting target to the real world coordinate of the mouse,
    // and them offsetting it by the screen coordinate, so we have it centered on the mouse :). 
    BeginTextureMode(ImageDisplayTexture);
        BeginMode2D(RenderDisplay->camera);
            ClearBackground(BLACK);  // Clear render texture background color
            DrawTexture(RenderDisplay->texture,0,0,WHITE);
            printf("Total: %u\n", TotalBbox);
            printf("Bboxes[0].x, h: %f,%f\n", Bboxes[0].Box.x,Bboxes[0].Box.width);
            // printf("Bboxes[1].x: %f\n", (Bboxes[1].Box.x));
            for (u32 BoxId = 0; BoxId < TotalBbox + 1; ++BoxId)
            {
                DrawRectangleLinesEx(Bboxes[BoxId].Box,15,LabelsColors[Bboxes[BoxId].Label]);
            }
            {
                // u32 x = (f32)Bboxes[CurrentBbox].Box.x;
                // u32 y = (f32)Bboxes[CurrentBbox].Box.y;
                // u32 w = (f32)Bboxes[CurrentBbox].Box.width;
                // u32 h = (f32)Bboxes[CurrentBbox].Box.height;
                // DrawRectangle(x - 2,y - 2,6,6,RAYWHITE);
                // DrawRectangle(x + w - 3,y - 2,6,6,RAYWHITE);
                // DrawRectangle(x + w - 3,y + h - 3,6,6,RAYWHITE);
                // DrawRectangle(x - 1,y + h - 3,6,6,RAYWHITE);
            }
        EndMode2D();
        DrawSegmentedLines(MousePositionRelative.x,MousePositionRelative.y,RenderWidth,RenderHeight,LabelsColors[0]);
    EndTextureMode();
}

internal
u32 Panel(u32 CurrentLabel)
{
    Vector2 MousePosition = GetMousePosition();
    u32 ScreenHeight = GetScreenHeight();

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

    return(CurrentLabel);
}

internal
void CameraRoll(render_display *RenderDisplay)
{
// Responsible to initialize new RenderDisplay

    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    
    f32 RenderWidth = ScreenWidth - PANELWIDTH;
    f32 RenderHeight = ScreenHeight;

    f32 InitialZoom = RenderWidth/RenderDisplay->texture .width - 0.005;
    Vector2 InitialOffSet = {(RenderWidth - RenderDisplay->texture .width*InitialZoom)*0.5f,(RenderHeight - RenderDisplay->texture.height*InitialZoom)*0.5f};
    RenderDisplay->camera = {InitialOffSet,{0,0},0,InitialZoom};

}

internal
void AnnotationPage(render_display *RenderDisplay)
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Common Annotation Menu initializations

    Vector2 MousePosition = GetMousePosition();
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    
    f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
    f32 FullImageDisplayHeight = ScreenHeight;
    f32 dt = GetFrameTime();

    internal u32 CurrentLabel = 0;
    internal bool first_frame = true;

    if (IsKeyPressed(KEY_R) || first_frame)
    {
        UnloadTexture(RenderDisplay->texture);
        const char *ImagePath = TextFormat("%s/%s",TEST_FOLDER_JPG, TEST_PIC);
        RenderDisplay->texture = LoadTexture(ImagePath);
        CameraRoll(RenderDisplay);
        first_frame = false;
    }
    
    // s32 count = 0;
    // const char *ImageName = TextSplit(TEST_PIC,'.',&count)[0];
    // const char *AnnPathTmp = TextFormat("%s/%s.ann",PROJECT_FOLDER, ImageName);
    // char* AnnPath = (char*)malloc(strlen(AnnPathTmp) + 1);
    // strcpy(AnnPath,AnnPathTmp);

    disp_mode DisplayMode = DispMode_creation;
    if (IsKeyPressed(KEY_D))
    {
        DisplayMode = DispMode_manipulation;
    }
    else if (IsKeyPressed(KEY_B))
    {
        DisplayMode = DispMode_creation;
    }

    internal RenderTexture2D ImageDisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
    SetTextureFilter(ImageDisplayTexture.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use
    if (IsWindowResized())
    {
        UnloadRenderTexture(ImageDisplayTexture);
        ImageDisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(ImageDisplayTexture.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use
        CameraRoll(RenderDisplay);

    }
    ImageDisplay(ImageDisplayTexture, RenderDisplay, DisplayMode);
    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

// Inside Lateral Menu Stuff & ImageDisplay Draw
    BeginDrawing();
        ClearBackground(PINK);

        DrawTexturePro(ImageDisplayTexture.texture, (Rectangle){ 0.0f, 0.0f, (float)ImageDisplayTexture.texture.width, (float)-ImageDisplayTexture.texture.height},
                (Rectangle){PANELWIDTH,0,FullImageDisplayWidth,FullImageDisplayHeight}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0,0,PANELWIDTH,ScreenHeight,DARKBLUE);
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
        // f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
        // f32 FullImageDisplayHeight = ScreenHeight;

        // f32 ImageDisplayWidth = FullImageDisplayWidth - IMAGEDISPLAYSIDEGAP;
        // f32 ImageDisplayHeight = ImageDisplayWidth*TextureRatio;

        // Rectangle ImageDisplayRec;
        // {
        //     f32 w = (float)CurrentTexture.width;
        //     f32 h = (float)CurrentTexture.height;

        //     Rectangle TextureRec = {Zoom.Position.x*w,Zoom.Position.y*h,w/Zoom.Strenght,h/Zoom.Strenght};
        //     ImageDisplayRec = {PANELWIDTH + IMAGEDISPLAYSIDEGAP/2,FullImageDisplayHeight/2 - ImageDisplayHeight/2,ImageDisplayWidth,ImageDisplayHeight};
        //     DrawTexturePro(CurrentTexture,TextureRec,ImageDisplayRec,(Vector2){0,0},0,WHITE);\
        // }

        
        // if (IsKeyDown(KEY_W))
        // {
        //     Zoom.Strenght += 0.9*dt;
        // }
        // if (IsKeyDown(KEY_S))
        // {
        //     Zoom.Strenght -= 0.9*dt;
        // }
        // if (IsKeyDown(KEY_LEFT))
        // {
        //     Zoom.Position.x -= 0.5*dt;
        // }
        // if (IsKeyDown(KEY_RIGHT))
        // {
        //     Zoom.Position.x += 0.5*dt;
        // }
        // if (IsKeyDown(KEY_UP))
        // {
        //     Zoom.Position.y -= 0.5*dt;
        // }
        // if (IsKeyDown(KEY_DOWN))
        // {
        //     Zoom.Position.y += 0.5*dt;
        // }
        // if (IsKeyPressed(KEY_D))
        // {
        //     DisplayMode = DispMode_manipulation;
        // }
        // else if (IsKeyPressed(KEY_B))
        // {
        //     DisplayMode = DispMode_creation;
        // }

        // if (CheckCollisionPointRec(MousePosition,(Rectangle){PANELWIDTH,0,FullImageDisplayWidth,FullImageDisplayHeight}))
        // {
        //     DrawSegmentedLines(MousePosition.x,MousePosition.y,ScreenWidth,ScreenHeight,LabelsColors[CurrentLabel]);

        //     switch (DisplayMode)
        //     {
        //         case DispMode_creation:
        //         {
        //             // Maybe this function is a bad idea and we should inline it
        //             TotalBbox = BoxCreation(TotalBbox,&CurrentBbox,CurrentGesture,CurrentLabel,MousePosition,Bboxes,AnnPath);
        //         break;
        //         }
        //         case DispMode_manipulation:
        //         {
        //             BoxManipulation(TotalBbox, CurrentGesture, MousePosition, Bboxes);
        //         break;
        //         }
        //     }
        // }
        // BeginScissorMode(ImageDisplayRec.x, ImageDisplayRec.y,ImageDisplayRec.width,ImageDisplayRec.height);
        //     for (u32 BoxId = 0; BoxId < TotalBbox + 1; ++BoxId)
        //     {
        //         DrawRectangleLinesEx(Bboxes[BoxId].Box,2,LabelsColors[Bboxes[BoxId].Label]);
        //     }
        //     {
        //         u32 x = (f32)Bboxes[CurrentBbox].Box.x;
        //         u32 y = (f32)Bboxes[CurrentBbox].Box.y;
        //         u32 w = (f32)Bboxes[CurrentBbox].Box.width;
        //         u32 h = (f32)Bboxes[CurrentBbox].Box.height;
        //         DrawRectangle(x - 2,y - 2,6,6,RAYWHITE);
        //         DrawRectangle(x + w - 3,y - 2,6,6,RAYWHITE);
        //         DrawRectangle(x + w - 3,y + h - 3,6,6,RAYWHITE);
        //         DrawRectangle(x - 1,y + h - 3,6,6,RAYWHITE);
        //     }
        // EndScissorMode();
        DrawFPS(10,10);
    EndDrawing();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}