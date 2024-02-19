#include "rlib_annotation_menu.h"

bool first_frame = true;
const global Color LabelsColors[10] = {RED,WHITE,GREEN,BLUE,MAGENTA,YELLOW,PURPLE,BROWN,SKYBLUE,LIME};
const global char *Labels[] = {"BOX", "STICKER", "COW", "DOG", "PNEUMOTORAX"};
bbox Bboxes[10] = {};
annotation_page_state AnnotationState = {};
annotation_display AnnotationDisplay = {};
u8 CurrentCursorSprite = {};

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
void BoxManipulation(Vector2 MousePosition)
{
    u32 CurrentGesture = GetGestureDetected();
    box_hit_state CollisionState = NoHit;
    f32 e = 8;
    s32 CollisionId = -1;

    for (u32 BoxId = 0; BoxId < AnnotationState.TotalBbox; ++BoxId)
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
            // SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
            CurrentCursorSprite = MOUSE_CURSOR_CROSSHAIR;
        break;
        }
        case InsideHit:
        {   
            internal Vector2 Tap = {};
            if (CurrentGesture & (GESTURE_TAP))
            {
                // SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
            }
            else if (CurrentGesture & (GESTURE_DRAG))
            {
                // SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
                Bboxes[CollisionId].Box.x += MousePosition.x - Tap.x;
                Bboxes[CollisionId].Box.y += MousePosition.y - Tap.y;
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
            }
            else if (CurrentGesture & (GESTURE_HOLD))
            {
                // SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;


            }
            else
            {
                // SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;

                
            }

        break;
        }
        case HorizontalHit:
        {
            // SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_EW;


        break;
        }
        case VerticalHit:
        {
            // SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_NS;
        break;
        }
    }
}

internal inline
void BoxCreation(Vector2 MousePosition)
{
    // SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    CurrentCursorSprite = MOUSE_CURSOR_CROSSHAIR;

    internal u32 PrevGesture = 3;
    Rectangle *BBox = &Bboxes[AnnotationState.TotalBbox].Box;
    Bboxes[AnnotationState.TotalBbox].Label = AnnotationState.CurrentLabel;
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
    if (BBox->width > 0.1 && ~(PrevGesture & (GESTURE_NONE)) && (CurrentGesture == (GESTURE_NONE)) && AnnotationState.TotalBbox <= 10)
    {
        AnnotationState.CurrentBbox = AnnotationState.TotalBbox;
        AnnotationState.TotalBbox += 1;
        // SaveDataToFile(AnnPath,Bboxes,TotalBbox);
        // ReadInMemoryAnn(AnnPath, TotalBbox);
    }
    PrevGesture = CurrentGesture;
}

internal
void RenderImageDisplay()
{
    f32 RenderWidth = AnnotationDisplay.DisplayTexture.texture.width;
    f32 RenderHeight = AnnotationDisplay.DisplayTexture.texture.height;
    Vector2 MousePosition = GetMousePosition();
    Vector2 MousePositionRelative = Vector2Clamp(MousePosition, {PANELWIDTH,0.0f}, {PANELWIDTH + RenderWidth, RenderHeight});
    MousePositionRelative.x -= PANELWIDTH;

    u32 CurrentGesture = GetGestureDetected();
    if ((CurrentGesture == GESTURE_DRAG) && (AnnotationState.DisplayMode == DispMode_moving))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/AnnotationDisplay.camera.zoom);

        AnnotationDisplay.camera.target = Vector2Add(AnnotationDisplay.camera.target, delta);
    }

    f32 wheel = GetMouseWheelMove();
    Vector2 mouseWorldPos = GetScreenToWorld2D(MousePositionRelative, AnnotationDisplay.camera);
    if (wheel != 0)
    {

        AnnotationDisplay.camera.offset = MousePositionRelative;
        AnnotationDisplay.camera.target = mouseWorldPos;

        const f32 zoomIncrement = 0.005f;
        const f32 zoomMin = 0.125f;

        AnnotationDisplay.camera.zoom += (wheel*zoomIncrement);
        if (AnnotationDisplay.camera.zoom < zoomMin) AnnotationDisplay.camera.zoom = zoomMin;
    }

    switch (AnnotationState.DisplayMode)
    {
        case DispMode_creation:
        {
            // Maybe this function is a bad idea and we should inline it
            BoxCreation(mouseWorldPos);
        break;
        }
        case DispMode_manipulation:
        {
            BoxManipulation(mouseWorldPos);
        break;
        }
        default:
        {
        break;
        }
    }

    //@NOTE(CAIO) I'm dumb and it took me a long time to understand how the camera 2D works...
    //So we have world coordinates (used in BeginMode2D) and screen coordinates. Target is the 
    //world coordinate for left top of the screen, but if we want we can use offset to move the target
    //to another point on the screen.. Our zoom now works by setting target to the real world coordinate of the mouse,
    // and them offsetting it by the screen coordinate, so we have it centered on the mouse :). 

    BeginTextureMode(AnnotationDisplay.DisplayTexture);
        BeginMode2D(AnnotationDisplay.camera);
            ClearBackground(BLACK);  // Clear render texture background color
            DrawTexture(AnnotationDisplay.ImageTexture,0,0,WHITE);
            printf("Total: %u\n", AnnotationState.TotalBbox);
            printf("Bboxes[0].x, h: %f,%f\n", Bboxes[0].Box.x,Bboxes[0].Box.width);

            for (u32 BoxId = 0; BoxId < AnnotationState.TotalBbox + 1; ++BoxId)
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
        DrawSegmentedLines(MousePositionRelative.x,MousePositionRelative.y,RenderWidth,RenderHeight,LabelsColors[AnnotationState.CurrentLabel]);
    EndTextureMode();
}

internal
u32 DrawPanel()
{
    Vector2 MousePosition = GetMousePosition();
    u32 ScreenHeight = GetScreenHeight();

    DrawRectangle(0,0,PANELWIDTH,ScreenHeight,DARKBLUE);
    internal s32 Active = {};
    GuiToggleGroup((Rectangle){0,120,150,20},TextJoin(Labels,ArrayCount(Labels),"\n"),&Active);
    AnnotationState.CurrentLabel = Active;

    for (u32 LabelId = 0; LabelId < ArrayCount(Labels);  ++LabelId)
    {   
        DrawRectangle(140,120 + 22*LabelId,10,20,LabelsColors[LabelId]);
    }

    if (IsKeyReleased(KEY_ONE))
    {
        AnnotationState.CurrentLabel = 0;
        Active = 0;
    }
    else if (IsKeyReleased(KEY_TWO))
    {
        AnnotationState.CurrentLabel = 1;
        Active = 1;

    }
    else if (IsKeyReleased(KEY_THREE))
    {
        AnnotationState.CurrentLabel = 2;
        Active = 2;

    }
    else if (IsKeyReleased(KEY_FOUR))
    {
        AnnotationState.CurrentLabel = 3;
        Active = 3;

    }
    else if (IsKeyReleased(KEY_FIVE))
    {
        AnnotationState.CurrentLabel = 4;
        Active = 4;

    }
    else if (IsKeyReleased(KEY_SIX))
    {
        AnnotationState.CurrentLabel = 5;
        Active = 5;
    }

    if (CheckCollisionPointRec(MousePosition,(Rectangle){0,0,PANELWIDTH,(float)ScreenHeight}))
    {
        // SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        CurrentCursorSprite = MOUSE_CURSOR_DEFAULT;
    }

    return(AnnotationState.CurrentLabel);
}

internal
void InitializeAnnotationDisplay()
{
// 
// Responsible to initialize new AnnotationDisplay
//
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    
    f32 RenderWidth = ScreenWidth - PANELWIDTH;
    f32 RenderHeight = ScreenHeight;

    f32 InitialZoom = RenderWidth/AnnotationDisplay.ImageTexture .width - 0.005;
    Vector2 InitialOffSet = {(RenderWidth - AnnotationDisplay.ImageTexture .width*InitialZoom)*0.5f,(RenderHeight - AnnotationDisplay.ImageTexture.height*InitialZoom)*0.5f};
    AnnotationDisplay.camera = {InitialOffSet,{0,0},0,InitialZoom};

}

internal
void AnnotationPage()
{
//
// Common Annotation Menu initializations
// 
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    
    f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
    f32 FullImageDisplayHeight = ScreenHeight;
// 
// First frame initialization and manual reset
// 
    if (IsKeyPressed(KEY_R) || first_frame)
    {
        AnnotationDisplay.DisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(AnnotationDisplay.DisplayTexture.texture, TEXTURE_FILTER_BILINEAR);
        UnloadTexture(AnnotationDisplay.ImageTexture);
        const char *ImagePath = TextFormat("%s/%s",TEST_FOLDER_JPG, TEST_PIC);
        AnnotationDisplay.ImageTexture = LoadTexture(ImagePath);
        InitializeAnnotationDisplay();

        s32 count = 0;
        const char *ImageName = TextSplit(TEST_PIC,'.',&count)[0];
        const char *AnnPathTmp = TextFormat("%s/%s.ann",PROJECT_FOLDER, ImageName);
        char* AnnPath = (char*)malloc(strlen(AnnPathTmp) + 1);
        strcpy(AnnPath,AnnPathTmp);

        first_frame = false;
    }
// 
// Reset texture when resizing
// 
    if (IsWindowResized())
    {
        UnloadRenderTexture(AnnotationDisplay.DisplayTexture);
        AnnotationDisplay.DisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(AnnotationDisplay.DisplayTexture.texture, TEXTURE_FILTER_BILINEAR);
        InitializeAnnotationDisplay();

    }
// 
// Getting global DisplayMode
// 
    if (IsKeyPressed(KEY_D))
    {
        AnnotationState.DisplayMode = DispMode_manipulation;
    }
    else if (IsKeyPressed(KEY_B))
    {
        AnnotationState.DisplayMode = DispMode_creation;
    }
    else if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        AnnotationState.DisplayMode = DispMode_moving;
    }
    else if (IsKeyReleased(KEY_LEFT_CONTROL))
    {
        AnnotationState.DisplayMode = DispMode_creation;
    }
// 
// Render Image Display
// 
    RenderImageDisplay();
// 
// Draw RenderImageDisplay and Panel
//  
    BeginDrawing();
        ClearBackground(PINK);
        DrawTexturePro(AnnotationDisplay.DisplayTexture.texture, (Rectangle){ 0.0f, 0.0f, (float)AnnotationDisplay.DisplayTexture.texture.width, (float)-AnnotationDisplay.DisplayTexture.texture.height},
                (Rectangle){PANELWIDTH,0,FullImageDisplayWidth,FullImageDisplayHeight}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawPanel();
        SetMouseCursor(CurrentCursorSprite);
        DrawFPS(10,10);
    EndDrawing();
}