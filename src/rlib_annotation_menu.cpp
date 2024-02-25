#include "rlib_annotation_menu.h"

#define MAX_STRINGS 16
#define MAX_LENGTH 16


const char *Labels[MAX_STRINGS] = {"BOX", "STICKER", "COW", "DOG", "PNEUMOTORAX"};
u32 LabelsTotal[MAX_STRINGS] = {};
const global Color LabelsColors[10] = {RED,WHITE,GREEN,BLUE,MAGENTA,YELLOW,PURPLE,BROWN,SKYBLUE,LIME};
bbox Bboxes[16] = {};

annotation_page_state AnnotationState = {};

annotation_display AnnotationDisplay = {};
bool first_frame = true;
bool ReloadImage = false;
u32 CurrentImageId = 0;
u8 CurrentCursorSprite = 0;

u32 CollisionState = NoHit;
bool isGrabbed = false;

//@TODO Make this better
internal
void DeleteBox(bbox Bboxes[], u32 *TotalBoxes, u32 CurrentBox) {
    for (int i = CurrentBox; i < *TotalBoxes; ++i) {
        Bboxes[i] = Bboxes[i + 1];
    }
    if (*TotalBoxes > 0) (*TotalBoxes)--;;
}

internal
void DrawSegmentedLines(const segmented_lines SegmentedLines)
{   
    const u32 gap = 4;
    const f32 SegLen = 12;

    f32 LenX = SegmentedLines.W;
    u32 NSegX = (u32)ceil(LenX/SegLen + gap);

    for (u32 Seg = 0; Seg < NSegX; ++Seg)
    {
        f32 start = Seg*(SegLen + gap) - SegLen/2;
        f32 end = start + SegLen;
        DrawLineEx({start,SegmentedLines.Y},{end,SegmentedLines.Y},2,SegmentedLines.color);
    }

    f32 LenY = SegmentedLines.H;
    u32 NSegY = (u32)ceil(LenY/SegLen + gap);

    for (u32 Seg = 0; Seg < NSegY; ++Seg)
    {
        f32 start = 0 + Seg*(SegLen + gap) - SegLen/2;
        f32 end = start + SegLen;
        DrawLineEx({SegmentedLines.X,start},{SegmentedLines.X,end},2,SegmentedLines.color);
    }
}

internal inline
void BoxManipulation(const Vector2 MousePosition)
{
    const f32 e = 50;

    if (IsKeyPressed(KEY_A))
    {
        printf("Delete\n");
        DeleteBox(Bboxes,&AnnotationState.TotalBbox, AnnotationState.CurrentBbox);
        if (AnnotationState.CurrentBbox > 0) AnnotationState.CurrentBbox--;
    }
    
    if (!isGrabbed)
    {
        for (u32 BoxId = 0; BoxId < AnnotationState.TotalBbox; ++BoxId)
        {
            if (CheckCollisionPointRec(MousePosition,Bboxes[BoxId].Box))
            {
                AnnotationState.CurrentBbox = BoxId;

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
                else
                {
                    CollisionState = InsideHit;
                }
                
                break;
            }
            else
            {
                CollisionState = NoHit;
            }
        }
    }

    switch (CollisionState)
    {
        case NoHit:
        {
            if (IsGestureHoldingOrDragging(AnnotationState.CurrentGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
            }
            else
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
            }
        break;
        }
        case InsideHit:
        {   
            internal Vector2 Tap = {};
            if (IsGestureTapped(AnnotationState.CurrentGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
                isGrabbed = true;
            }
            else if (IsGestureDragging(AnnotationState.CurrentGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
                Bboxes[AnnotationState.CurrentBbox].Box.x += MousePosition.x - Tap.x;
                Bboxes[AnnotationState.CurrentBbox].Box.y += MousePosition.y - Tap.y;
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
            }

            else if (IsGestureReleased(AnnotationState.CurrentGesture, AnnotationState.PrevGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
                isGrabbed = false;
            }
            else if (AnnotationState.CurrentGesture & (GESTURE_HOLD))
            {
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
            }
            else
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
            }
        break;
        }

        case HorizontalHit:
        {
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_EW;
        break;
        }

        case VerticalHit:
        {
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_NS;
        break;
        }
    }
}

internal inline
void BoxCreation(const Vector2 MousePosition)
{
    CurrentCursorSprite = MOUSE_CURSOR_CROSSHAIR;

    Rectangle *BBox = &Bboxes[AnnotationState.CurrentBbox].Box;
    Bboxes[AnnotationState.CurrentBbox].Label = AnnotationState.CurrentLabel;
    internal Vector2 Tap = {};

    if (AnnotationState.CurrentGesture & (GESTURE_TAP))
    {
        Tap.x = MousePosition.x;
        Tap.y = MousePosition.y;
        AnnotationState.TotalBbox += 1;
    }
    else
    {
        if (AnnotationState.CurrentGesture & (GESTURE_DRAG|GESTURE_HOLD))
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
    if (BBox->width > 0.1 && ~(AnnotationState.PrevGesture & (GESTURE_NONE)) && 
       (AnnotationState.CurrentGesture == (GESTURE_NONE)) && AnnotationState.TotalBbox < ArrayCount(Bboxes) - 1)
    {
        AnnotationState.CurrentBbox = AnnotationState.TotalBbox;
        LabelsTotal[AnnotationState.CurrentLabel] += 1;

        // SaveDataToFile(AnnPath,Bboxes,TotalBbox);
        // ReadInMemoryAnn(AnnPath, TotalBbox);
    }
    AnnotationState.PrevGesture = AnnotationState.CurrentGesture;
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
    AnnotationState.PrevGesture = AnnotationState.CurrentGesture; // This is used on BoxCreation & BoxManipulation


    //@NOTE(CAIO) I'm dumb and it took me a long time to understand how the camera 2D works...
    //So we have world coordinates (used in BeginMode2D) and screen coordinates. Target is the 
    //world coordinate for left top of the screen, but if we want we can use offset to move the target
    //to another point on the screen.. Our zoom now works by setting target to the real world coordinate of the mouse,
    // and them offsetting it by the screen coordinate, so we have it centered on the mouse :). 

    BeginTextureMode(AnnotationDisplay.DisplayTexture);
        BeginMode2D(AnnotationDisplay.camera);
            ClearBackground(BLACK);  // Clear render texture background color
            DrawTexture(AnnotationDisplay.ImageTexture,0,0,WHITE);

            //@TODO Maybe we do a fragment shader for rectangle drawing
            printf("Total: %u\n", AnnotationState.TotalBbox);

            for (u32 BoxId = 0; BoxId < AnnotationState.TotalBbox; ++BoxId)
            {
                DrawRectangleLinesEx(Bboxes[BoxId].Box,15,LabelsColors[Bboxes[BoxId].Label]);
            }

            DrawRectangleRec(Bboxes[AnnotationState.CurrentBbox].Box,WHITE);
            {
                // u32 x = (f32)Bboxes[AnnotationState.CurrentBbox].Box.x;
                // u32 y = (f32)Bboxes[AnnotationState.CurrentBbox].Box.y;
                // u32 w = (f32)Bboxes[AnnotationState.CurrentBbox].Box.width;
                // u32 h = (f32)Bboxes[AnnotationState.CurrentBbox].Box.height;
                // DrawRectangle(x - 2,y - 2,6,6,RAYWHITE);
                // DrawRectangle(x + w - 3,y - 2,6,6,RAYWHITE);
                // DrawRectangle(x + w - 3,y + h - 3,6,6,RAYWHITE);
                // DrawRectangle(x - 1,y + h - 3,6,6,RAYWHITE);
            }
        EndMode2D();
        DrawSegmentedLines((segmented_lines){MousePositionRelative.x,MousePositionRelative.y,RenderWidth,RenderHeight,LabelsColors[AnnotationState.CurrentLabel]});
    EndTextureMode();
}

internal
u32 DrawPanel()
{
    Vector2 MousePosition = GetMousePosition();
    u32 ScreenHeight = GetScreenHeight();
 
    DrawRectangle(0,0,PANELWIDTH,ScreenHeight,DARKBLUE);

    const u32 LabelsW = ceil(PANELWIDTH*0.8);
    const u32 LabelsH = 30;
    const u32 LabelsColorW = 20;
    const u32 LabelGroupLoc = 120;
    const u32 LabelFontSize = 20;
    u32 LabelNWHeight = 10;

// 
// Draw Buttons
//

    //@TODO Improve this.. maybe we do this computation not in runtime
    bool toggle = false;
    char ButtonsText[2][40] = {};
    TextCopy(ButtonsText[0],GuiIconText(ICON_BOX, "Annotation Mode"));
    TextCopy(ButtonsText[1],GuiIconText(ICON_CURSOR_SCALE, "Edit Mode"));
    Rectangle bounds[2] = {(Rectangle){ 0, 90, 140, 20 }, (Rectangle){ 140, 90, 140, 20 }};
    for (u32 i = 0; i < ArrayCount(ButtonsText); i++)
    {
        if (i == (AnnotationState.DisplayMode))
        {
            toggle = true;
            GuiToggle(bounds[i], ButtonsText[i], &toggle);
        }
        else
        {
            toggle = false;
            GuiToggle(bounds[i], ButtonsText[i], &toggle);
            if (toggle) AnnotationState.DisplayMode = i;
        }
    }


// 
// Draw Labels on Panel
// 
    for (u32 LabelId = 0; LabelId < ArrayCount(Labels);  ++LabelId)
    {   
        u32 N = LabelsTotal[LabelId];
        char N_str[20];
        sprintf(N_str, "%u", N);
        u32 LabelNWidth = MeasureText(N_str, LabelFontSize);

        u32 LabelY = LabelGroupLoc + (LabelsH + GuiGetStyle(TOGGLE, GROUP_PADDING))*LabelId;
        u32 TextLocX = (u32)ceil(LabelsW + LabelsColorW + LabelsH/2.0f - LabelNWidth/2.0f);
        u32 TextLocY = (u32)ceil(LabelY + LabelNWHeight/2.0f);

        DrawRectangle(LabelsW, LabelY, LabelsColorW, LabelsH, LabelsColors[LabelId]);

        DrawRectangle(LabelsW + LabelsColorW, LabelY, LabelsH, LabelsH, LIGHTGRAY);
        DrawText(N_str, TextLocX, TextLocY, LabelFontSize, BLACK);
    }
//
//  Get Current Label from Panel interaction 
//
    GuiToggleGroup((Rectangle){0,LabelGroupLoc,(f32)LabelsW,LabelsH},TextJoin(Labels,ArrayCount(Labels),"\n"),&AnnotationState.CurrentLabel);

    if (IsKeyReleased(KEY_ONE))
    {
        AnnotationState.CurrentLabel = 0;
    }
    else if (IsKeyReleased(KEY_TWO))
    {
        AnnotationState.CurrentLabel = 1;

    }
    else if (IsKeyReleased(KEY_THREE))
    {
        AnnotationState.CurrentLabel = 2;

    }
    else if (IsKeyReleased(KEY_FOUR))
    {
        AnnotationState.CurrentLabel = 3;

    }
    else if (IsKeyReleased(KEY_FIVE))
    {
        AnnotationState.CurrentLabel = 4;

    }
    else if (IsKeyReleased(KEY_SIX))
    {
        AnnotationState.CurrentLabel = 5;
    }

    if (CheckCollisionPointRec(MousePosition,(Rectangle){0,0,PANELWIDTH,(float)ScreenHeight}))
    {
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
void AnnotationPage(FilePathList PathList)
{
//
// Common Annotation Menu initializations
// 
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    AnnotationState.CurrentGesture = GetGestureDetected();
    
    f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
    f32 FullImageDisplayHeight = ScreenHeight;

    // printf("Current: %u\n", AnnotationState.CurrentBbox);
// 
// Handle Input Events 
//  
    if (IsKeyPressed(KEY_R))
    {
        ReloadImage = true;
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
        ReloadImage = true;
        CurrentImageId += CurrentImageId < PathList.count - 1 ? 1 : 0;
    }
    else if (IsKeyPressed(KEY_LEFT))
    {
        ReloadImage = true;
        CurrentImageId -= CurrentImageId > 0 ? 1 : 0;

    }
    char *ImagePath = PathList.paths[CurrentImageId];
    
// 
// First frame initialization and manual reset
// 
    if (ReloadImage || first_frame)
    {
        AnnotationDisplay.DisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(AnnotationDisplay.DisplayTexture.texture, TEXTURE_FILTER_BILINEAR);
        UnloadTexture(AnnotationDisplay.ImageTexture);
        // const char *ImagePath = TextFormat("%s/%s",TEST_FOLDER_JPG, TEST_PIC);
        AnnotationDisplay.ImageTexture = LoadTexture(ImagePath);
        InitializeAnnotationDisplay();

        // s32 count = 0;
        // const char *ImageName = TextSplit(TEST_PIC,'.',&count)[0];
        // const char *AnnPathTmp = TextFormat("%s/%s.ann",PROJECT_FOLDER, ImageName);
        // char* AnnPath = (char*)malloc(strlen(AnnPathTmp) + 1);
        // strcpy(AnnPath,AnnPathTmp);

        first_frame = false;
        ReloadImage = false;
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
        CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
    }
    // There is probably a better way to do this.. bc now this is being called all the time
    else if (!(IsKeyDown(KEY_LEFT_CONTROL)) && (AnnotationState.CurrentGesture == GESTURE_NONE) &&
            (AnnotationState.DisplayMode == DispMode_moving))
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
        DrawPanel(); // Panel After RenderImageDisplay otherwise it breaks CursorSprite
        SetMouseCursor(CurrentCursorSprite); 
        DrawFPS(10,10);
    EndDrawing();
}