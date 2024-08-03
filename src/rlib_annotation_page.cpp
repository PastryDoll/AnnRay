#include "rlib_annotation_page.h"

//
// Per project
//

// char Labels[MAX_LENGTH][MAX_STRINGS] = {"BOX", "STICKER", "COW", "DOG", "PNEUMOTORAX"};
char Labels[MAX_LENGTH][MAX_STRINGS] = {};
// u32 TotalLabels = {};
const global Color LabelsColors[10] = {RED,WHITE,GREEN,BLUE,MAGENTA,YELLOW,PURPLE,BROWN,SKYBLUE,LIME}; //@TODO Randomize the colors in a nice way;
global bool IsTextureReady2 = false; //Image texture that is fectch async 
//
// Per Image
//
bboxes Bboxes = {};
char* CurrentAnnFile = NULL;

annotation_page_state AnnotationState = {};
annotation_display AnnotationDisplay = {};
bool first_frame = true;
bool ReloadImage = false;
u32 CurrentImageId = 0;
u32 TotalImages = 0;

u32 CollisionState = NoHit;
bool isBoxGrabbed = false;
// bool isBoxDrawing = false;

//@TODO Make this better.  This is actually broken.. we need to be carefull of what we delete.. take a look on this.
internal
void DeleteBox(bboxes *Bboxes, u32 BoxToDelete) 
{
    if (BoxToDelete <  Bboxes->TotalBoxes)
    {
        u32 LabelToDelete = Bboxes->Boxes[BoxToDelete].Label;
        if (Bboxes->TotalBoxes > 0) (Bboxes->TotalBoxes)--;
        if ((Bboxes->LabelsCount[LabelToDelete] > 0)) (Bboxes->LabelsCount[LabelToDelete])--;

        for (u32 i = BoxToDelete; i < Bboxes->TotalBoxes + 1; ++i) {
            Bboxes->Boxes[i] = Bboxes->Boxes[i + 1];
        }
        AnnotationState.CurrentBbox = Bboxes->TotalBoxes;
    }
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

// @TODO CLAMP BBOX TO IMAGE
internal inline
void BoxManipulation(const Vector2 MousePosition)
{
    const f32 e = 50;
    u32 CurrentBbox = 0;
    AnnotationState.FocusMode = (AnnotationState.CurrentBbox != Bboxes.TotalBoxes);

    if (IsKeyPressed(KEY_A))
    {
        DeleteBox(&Bboxes, AnnotationState.CurrentBbox);
    }
    
    if (!isBoxGrabbed)
    {
        //@TODO we need to accoutn for when we have overlaying of boxes
        //@TODO Check for corners also
        for (u32 BoxId = 0; BoxId < Bboxes.TotalBoxes; ++BoxId)
        {
            if (CheckCollisionPointRec(MousePosition,Bboxes.Boxes[BoxId].Box))
            {
                CurrentBbox = BoxId;
                // Right Logic || Left Logic
                if  (MousePosition.x > (Bboxes.Boxes[BoxId].Box.x + Bboxes.Boxes[BoxId].Box.width - e))
                {
                    CollisionState = HorizontalRightHit;
                }
                else if (MousePosition.x < (Bboxes.Boxes[BoxId].Box.x + e))
                {
                    CollisionState = HorizontalLeftHit;
                }
                // Botton Logic || Top Logic
                else if (MousePosition.y > (Bboxes.Boxes[BoxId].Box.y + Bboxes.Boxes[BoxId].Box.height - e)) 
                {
                    CollisionState = VerticalBottomHit;
                }
                else if (MousePosition.y < (Bboxes.Boxes[BoxId].Box.y + e))
                {
                    CollisionState = VerticalTopHit;
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
            if (IsGestureTapped(AnnotationState.CurrentGesture))
            {
                AnnotationState.CurrentBbox = Bboxes.TotalBoxes;
                AnnotationState.FocusMode = false;
            }
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
                AnnotationState.CurrentBbox = CurrentBbox;
                isBoxGrabbed = true;
            }
            else if (IsGestureDragging(AnnotationState.CurrentGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_RESIZE_ALL;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.x += MousePosition.x - Tap.x;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.y += MousePosition.y - Tap.y;
                Tap.x = MousePosition.x;
                Tap.y = MousePosition.y;
            }

            else if (IsGestureReleased(AnnotationState.CurrentGesture, AnnotationState.PrevGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
                isBoxGrabbed = false;
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
        case HorizontalRightHit:
        {
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_EW;
            if (IsGestureTapped(AnnotationState.CurrentGesture))
            {
                AnnotationState.CurrentBbox = CurrentBbox;
                isBoxGrabbed = true;
            }
            if (IsGestureDragging(AnnotationState.CurrentGesture))
            {
                f32 DeltaX = GetMouseDelta().x;
                DeltaX = 1.0f/AnnotationDisplay.camera.zoom*DeltaX;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.width += DeltaX;
            }
            else if (IsGestureReleased(AnnotationState.CurrentGesture, AnnotationState.PrevGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
                isBoxGrabbed = false;
            }
        break;
        }
        case HorizontalLeftHit:
        {
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_EW;
            if (IsGestureTapped(AnnotationState.CurrentGesture))
            {
                AnnotationState.CurrentBbox = CurrentBbox;
                isBoxGrabbed = true;
            }
            if (IsGestureDragging(AnnotationState.CurrentGesture))
            {
                f32 DeltaX = GetMouseDelta().x;
                DeltaX = 1.0f/AnnotationDisplay.camera.zoom*DeltaX;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.width -= DeltaX;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.x += DeltaX;
            }
            else if (IsGestureReleased(AnnotationState.CurrentGesture, AnnotationState.PrevGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
                isBoxGrabbed = false;
            }
        break;
        }
        case VerticalTopHit:
        {
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_NS;
            if (IsGestureTapped(AnnotationState.CurrentGesture))
            {
                AnnotationState.CurrentBbox = CurrentBbox;
                isBoxGrabbed = true;
            }
            if (IsGestureDragging(AnnotationState.CurrentGesture))
            {
                f32 DeltaY = GetMouseDelta().y;
                DeltaY = 1.0f/AnnotationDisplay.camera.zoom*DeltaY;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.height -= DeltaY;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.y += DeltaY;
            }
            else if (IsGestureReleased(AnnotationState.CurrentGesture, AnnotationState.PrevGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
                isBoxGrabbed = false;
            }

        break;
        }
        case VerticalBottomHit:
        {
            CurrentCursorSprite = MOUSE_CURSOR_RESIZE_NS;
            if (IsGestureTapped(AnnotationState.CurrentGesture))
            {
                AnnotationState.CurrentBbox = CurrentBbox;
                isBoxGrabbed = true;
            }
            if (IsGestureDragging(AnnotationState.CurrentGesture))
            {
                f32 DeltaY = GetMouseDelta().y;
                DeltaY = 1.0f/AnnotationDisplay.camera.zoom*DeltaY;
                Bboxes.Boxes[AnnotationState.CurrentBbox].Box.height += DeltaY;
            }
            else if (IsGestureReleased(AnnotationState.CurrentGesture, AnnotationState.PrevGesture))
            {
                CurrentCursorSprite = MOUSE_CURSOR_POINTING_HAND;
                isBoxGrabbed = false;
            }
        break;
        }
    }
}

internal inline
void BoxCreation(const Vector2 MousePosition)
{
    CurrentCursorSprite = MOUSE_CURSOR_CROSSHAIR;

    Rectangle *BBox = &Bboxes.Boxes[Bboxes.TotalBoxes].Box;
    Bboxes.Boxes[Bboxes.TotalBoxes].Label = AnnotationState.CurrentLabel;
    internal Vector2 Tap = {};

    if (IsGestureTapped(AnnotationState.CurrentGesture))
    {
        Tap.x = MousePosition.x;
        Tap.y = MousePosition.y;
        AnnotationState.FocusMode = false;
    }
    else
    {
        if (IsGestureDragging(AnnotationState.CurrentGesture))
        {
            AnnotationState.CurrentBbox = Bboxes.TotalBoxes;
            AnnotationState.FocusMode = true;
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
    if (BBox->width*BBox->height > MinBboxArea && (Bboxes.TotalBoxes < MAX_TOTAL_BOXES-1) && IsGestureReleased(AnnotationState.CurrentGesture,AnnotationState.PrevGesture))
    {
        Bboxes.TotalBoxes += 1;
        Bboxes.LabelsCount[AnnotationState.CurrentLabel] += 1;
        // AnnotationState.CurrentBbox = Bboxes.TotalBoxes;
    }
}

internal
void DrawRenderImageDisplay()
{
    //@NOTE(CAIO) I'm dumb and it took me a long time to understand how the camera 2D works...
    //So we have world coordinates (used in BeginMode2D) and screen coordinates. Target is the 
    //world coordinate for left top of the screen, but if we want we can use offset to move the target
    //to another point on the screen.. Our zoom now works by setting target to the real world coordinate of the mouse,
    // and them offsetting it by the screen coordinate, so we have it centered on the mouse :). 

    BeginTextureMode(AnnotationDisplay.DisplayTexture);
        BeginMode2D(AnnotationDisplay.camera);
            ClearBackground(BLACK); 
            DrawText("Loading...", (AnnotationDisplay.DisplayTexture.texture.width - MeasureText("Loading...", 50))*0.5f,AnnotationDisplay.DisplayTexture.texture.height*0.5f - 50, 50, RED);
            if (IsTextureReady2)
            {
                f32 ImageWidth = AnnotationDisplay.ImageTexture.width;
                f32 ImageHeight = AnnotationDisplay.ImageTexture.height;
                BeginShaderMode(shaders);
                    Rectangle CurrBox = Bboxes.Boxes[AnnotationState.CurrentBbox].Box;
                    Rectangle NormalizedCurrBox = (Rectangle){CurrBox.x/ImageWidth, CurrBox.y/ImageHeight, CurrBox.width/ImageWidth, CurrBox.height/ImageHeight};
                    SetShaderValue(shaders, FragBoxLoc, &NormalizedCurrBox, SHADER_UNIFORM_VEC4);
                    SetShaderValue(shaders, FragActiveLoc, &AnnotationState.FocusMode, SHADER_UNIFORM_INT);
                    DrawTexture(AnnotationDisplay.ImageTexture,0,0,WHITE);
                EndShaderMode();

                //@TODO Maybe we do a fragment shader for rectangle drawing
                for (u32 BoxId = 0; BoxId < Bboxes.TotalBoxes+1; ++BoxId)
                {
                    Bboxes.Boxes[BoxId].Box = GetCollisionRec(Bboxes.Boxes[BoxId].Box,{0,0,(f32)AnnotationDisplay.ImageTexture.width,(f32)AnnotationDisplay.ImageTexture.height});
                    DrawRectangleLinesEx(Bboxes.Boxes[BoxId].Box,2/AnnotationDisplay.camera.zoom,LabelsColors[Bboxes.Boxes[BoxId].Label]);
                }
            }
        EndMode2D();
    EndTextureMode();
}

internal 
void ResetImage()
{   
    Bboxes = {0};
    SaveAnnToFile(CurrentAnnFile,&Bboxes);
    return;
};

internal
u32 DrawLeftPanel(u32 TotalLabels)
{
    Vector2 MousePosition = GetMousePosition();
    u32 ScreenHeight = GetScreenHeight();
    u32 ScreenWidth = GetScreenWidth();
    if (CheckCollisionPointRec(MousePosition,(Rectangle){0,0,PANELWIDTH,(float)ScreenHeight})) CurrentCursorSprite = MOUSE_CURSOR_DEFAULT;
 
    DrawRectangle(0,0,PANELWIDTH,ScreenHeight,DARKBLUE);

    const u32 LabelsW = ceil(PANELWIDTH*0.8);
    const u32 LabelsH = 30;
    const u32 LabelsColorW = 20;
    const u32 LabelGroupLoc = 120;
    const u32 LabelFontSize = 20;
    u32 LabelNWHeight = 10;

// 
// Draw Labels on Panel
// 
    GuiButton({100,500,100,100},GuiIconText(ICON_CROSS_SMALL, ""));
    // GuiDrawText(GuiIconText(ICON_CROSS_SMALL, ""),{100,500,100,100},0,RED);
    for (u32 LabelId = 0; LabelId < TotalLabels;  ++LabelId)
    {   
        u32 N = Bboxes.LabelsCount[LabelId];
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
    
    // Char array to char * array
    const char *LabelsJoinedText[TotalLabels];
    for (u32 LabelId = 0; LabelId < TotalLabels; ++LabelId)
    {
        const char *LabelPtr = Labels[LabelId];
        LabelsJoinedText[LabelId] = LabelPtr;
    }
    Rectangle LabelGroupRec = (Rectangle){0,LabelGroupLoc,(float)LabelsW,LabelsH};

    GuiToggleGroup(LabelGroupRec,TextJoin(LabelsJoinedText,TotalLabels,"\n"),&AnnotationState.CurrentLabel);

// 
// Write new labels
// 

    // Add + to the end 
    //@TODO Factor this to a function
    {
        Rectangle NewLabelRec = LabelGroupRec;
        NewLabelRec.y += (GuiGetStyle(TOGGLE, GROUP_PADDING) + LabelsH)*TotalLabels;
        internal bool clicked = false;
        internal bool writing = false;
        internal bool Active = true;
        internal char newLabelName[MAX_LENGTH] = "\0";      // NOTE: One extra space required for null terminator char '\0'
        internal u32 letterCount = 0;
        
        if (!writing)
        {
            if(GuiButton(NewLabelRec,"+")) 
            {
                clicked = true;
                Active = true;
            }
        }
        if (clicked)
        {
            writing = true;
            TextInputBox(NewLabelRec, &Active, &CurrentCursorSprite, newLabelName, &letterCount, MAX_LENGTH,20);
            DrawText(newLabelName, (u32)NewLabelRec.x + 5, (u32)NewLabelRec.y + 5, 20, BLACK);


        }
        if (IsKeyPressed(KEY_ENTER) && (writing))
        {
            Active = false;
            writing = false;
            clicked = false;
            printf("New Label: %s\n", newLabelName);
            strcpy(Labels[TotalLabels], newLabelName);
            strcpy(newLabelName,"");
            letterCount = 0;
            TotalLabels += 1;
            // SaveAnnToFile(AnnPath,&Bboxes);
            SaveLabelsToFile(Labels, TotalLabels);
        }
    }

//
//  Get Current Label from Panel interaction 
//

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
    else if (IsKeyReleased(KEY_SEVEN))
    {
        AnnotationState.CurrentLabel = 6;
    }
    else if (IsKeyReleased(KEY_EIGHT))
    {
        AnnotationState.CurrentLabel = 7;
    }
    else if (IsKeyReleased(KEY_NINE))
    {
        AnnotationState.CurrentLabel = 8;
    }
    else if (IsKeyReleased(KEY_ZERO))
    {
        AnnotationState.CurrentLabel = 9;
    }

// 
// Draw Buttons
//

    // Modes
    //@TODO Improve this.. maybe we do this computation not in runtime
    char ButtonsText[2][40] = {};
    TextCopy(ButtonsText[0],GuiIconText(ICON_BOX, "Annotation Mode"));
    TextCopy(ButtonsText[1],GuiIconText(ICON_CURSOR_SCALE, "Edit Mode"));
    Rectangle bounds[2] = {(Rectangle){ 0, 90, 140, 20 }, (Rectangle){ 140, 90, 140, 20 }};
    bool toggle = false;
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
//// Draw Current Image/ Total images display
// 
    {
        const char *Text = TextFormat("%u/%u", CurrentImageId+1, TotalImages);
        DrawText(Text,(PANELWIDTH - MeasureText(Text,30))*0.5f, ScreenHeight - 32,30, RED);
    }

// 
//// Reset
// 
    {
        internal bool waiting = false;
        if (GuiButton({PANELWIDTH*0.8,10,PANELWIDTH*0.19,30},"Reset")) waiting = true;
        if (waiting)
        {
            const char* buttons = "YES; NO";
            s32 clicked = GuiMessageBox({(f32)ScreenWidth/2 - 100,(f32)ScreenHeight/2 - 75, 400, 150},"Reset Image", "Are you sure you want to delete all labels of this image ?",buttons);
            if (clicked == 0) waiting = false;
            else if (clicked == 1) ResetImage(), waiting = false;
            else if (clicked == 2) waiting = false;
        }
    }

    return(TotalLabels);
}

internal
u32 InitializeAnnotationDisplayAndState(char* CurrentAnnFile, annotation_page_state *State)
{
// 
// Responsible to initialize new AnnotationDisplay
//
    {   
        // 
        //// Initialize annotation display
        // 

        u32 ScreenWidth = GetScreenWidth();
        u32 ScreenHeight = GetScreenHeight();
        
        f32 RenderWidth = ScreenWidth - PANELWIDTH;
        f32 RenderHeight = ScreenHeight;

        // if (ImageReady)
        // {

            f32 InitialZoom = RenderWidth/AnnotationDisplay.ImageTexture.width - 0.005;
            Vector2 InitialOffSet = {(RenderWidth - AnnotationDisplay.ImageTexture.width*InitialZoom)*0.5f,(RenderHeight - AnnotationDisplay.ImageTexture.height*InitialZoom)*0.5f};
            AnnotationDisplay.camera = {InitialOffSet,{0,0},0,InitialZoom};
        // }
    }
    
    u32 TotalLabels = ReadLabelsFromFile(Labels);
    TotalLabels = ReadAnnFromFile(CurrentAnnFile,&Bboxes);
    AnnotationState.CurrentBbox = Bboxes.TotalBoxes;
    return TotalLabels;
}

internal
u32 AnnotationPage(FilePathList PathList, thread_info_image *AnnThreadInfo)
{
//
// Common Annotation Menu initializations
// 
    // printf("Image? %u\n", AnnThreadInfo->ImageReady);
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    AnnotationState.CurrentGesture = GetGestureDetected();
    
    f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
    f32 FullImageDisplayHeight = ScreenHeight;

// 
// Handle Input Events 
//  
    if (IsKeyPressed(KEY_R) && IsTextureReady2)
    {
        ReloadImage = true;
        ResetImage();
    }
    else if (IsKeyPressed(KEY_RIGHT) && IsTextureReady2)
    {
        ReloadImage = true;
        CurrentImageId += CurrentImageId < PathList.count - 1 ? 1 : 0;
    }
    else if (IsKeyPressed(KEY_LEFT) && IsTextureReady2)
    {
        ReloadImage = true;
        CurrentImageId -= CurrentImageId > 0 ? 1 : 0;
    }
    
// 
// First frame initialization and manual reset
// 
    internal u32 TotalLabels = 0;
    if (ReloadImage || first_frame)
    {
        TotalImages = PathList.count;
        char *ImagePath = PathList.paths[CurrentImageId];
        UnloadTexture(AnnotationDisplay.ImageTexture);
        UnloadRenderTexture(AnnotationDisplay.DisplayTexture);
        AnnotationDisplay.DisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(AnnotationDisplay.DisplayTexture.texture, TEXTURE_FILTER_BILINEAR);
        AnnotationDisplay.camera = {{0,0}, {0,0}, 0, 1.0f};
        RequestImageAsync(AnnThreadInfo, ImagePath);
        IsTextureReady2 = false;


        // Save last_project worked
        SaveFileText("../projects/last_project.txt", GlobalState.ProjectName);

        first_frame = false;
        ReloadImage = false;
    }

    if(AnnThreadInfo->ImageReady)
    {
        char *ImagePath = PathList.paths[CurrentImageId];
        AnnotationDisplay.ImageTexture = LoadTextureFromImage(AnnThreadInfo->AsyncImage);
        UnloadImage(AnnThreadInfo->AsyncImage);
        AnnThreadInfo->ImageReady = false;
        IsTextureReady2 = true;
        {
            s32 count = 0;
            const char *ImageName = TextSplit(ImagePath,'/',&count)[count-1];
            const char *AnnFileTmp = TextFormat("%s.ann", ImageName);
            if (CurrentAnnFile) free(CurrentAnnFile);
            CurrentAnnFile = (char*)malloc(strlen(AnnFileTmp) + 1);
            strcpy(CurrentAnnFile,AnnFileTmp);
            TotalLabels = InitializeAnnotationDisplayAndState(CurrentAnnFile,&AnnotationState);
        }
    }

// 
// Reset texture when resizing
// 
    if (IsWindowResized())
    {
        UnloadRenderTexture(AnnotationDisplay.DisplayTexture);
        AnnotationDisplay.DisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(AnnotationDisplay.DisplayTexture.texture, TEXTURE_FILTER_BILINEAR);
        TotalLabels = InitializeAnnotationDisplayAndState(CurrentAnnFile,&AnnotationState);
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
// Annotation Display Texture Camera 
// 
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

// 
//// Display Mode Selection
// 

    if (IsTextureReady2)
    {
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
    }
    AnnotationState.PrevGesture = AnnotationState.CurrentGesture; // This is used on BoxCreation & BoxManipulation


// 
// Draw Render Image Display
// 
    DrawRenderImageDisplay();

// 
// Draw RenderImageDisplay and Panel
//  
    BeginDrawing();
        ClearBackground(PINK);
            DrawTexturePro(AnnotationDisplay.DisplayTexture.texture, (Rectangle){ 0.0f, 0.0f, (float)AnnotationDisplay.DisplayTexture.texture.width, (float)-AnnotationDisplay.DisplayTexture.texture.height},
                    (Rectangle){PANELWIDTH,0,FullImageDisplayWidth,FullImageDisplayHeight}, (Vector2){ 0, 0 }, 0.0f, WHITE);
        DrawSegmentedLines((segmented_lines){MousePosition.x,MousePosition.y,RenderWidth,RenderHeight,LabelsColors[AnnotationState.CurrentLabel]});
        TotalLabels = DrawLeftPanel(TotalLabels); // Panel After RenderImageDisplay otherwise it breaks CursorSprite

        // Back to Front Page

        {
            if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        }


        SetMouseCursor(CurrentCursorSprite); 

// 
// Draw Debug info:
// 
        DrawText(TextFormat("CurrentBox: %u",AnnotationState.CurrentBbox),10,30,10,WHITE);
        DrawText(TextFormat("TotalBoxes: %u",Bboxes.TotalBoxes),10,40,10,WHITE);
        DrawText(TextFormat("CurrentLabel: %u",AnnotationState.CurrentLabel),10,50,10,WHITE);
        DrawText(TextFormat("Zoom: %f",AnnotationDisplay.camera.zoom),10,60,10,WHITE);
        DrawText(TextFormat("TotalLabels: %u",TotalLabels),10,70,10,WHITE);
        // DrawText(TextFormat("CurrentBox: %u",AnnotationState.CurrentBbox),10,30,10,WHITE);
        DrawFPS(10,10);

    EndDrawing();


    if (CurrentAnnFile)
    {
        TotalLabels = SaveAnnToFile(CurrentAnnFile,&Bboxes); //@SpeedUp Dont call this every
        SaveLabelsToFile(Labels, TotalLabels);
        assert(TotalLabels > 0);
    }

    return ANNOTATION_PAGE;
}