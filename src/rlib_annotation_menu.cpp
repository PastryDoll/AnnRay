#include "rlib_annotation_menu.h"

const global Color LabelsColors[10] = {RED,WHITE,GREEN,BLUE,MAGENTA,YELLOW,PURPLE,BROWN,SKYBLUE,LIME};
const global char *Labels[] = {"BOX", "STICKER", "COW", "DOG", "PNEUMOTORAX"};

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
void ImageDisplay(bool ShouldLoadTexture, const char *TexturePath,  RenderTexture2D ImageDisplayTexture, Vector2 position, f32 Zoom, f32 *TextureRatio)
{
    internal Texture texture;
    // internal RenderTexture2D ImageDisplayTexture;
    internal bool ShouldLoadTextures = true;
    if (ShouldLoadTextures)
    {
        texture = LoadTexture(TexturePath);
        *TextureRatio = (float)texture.height/texture.width;

        ShouldLoadTextures = false;
    }

    Camera2D camera;
    camera.offset = (Vector2){(ImageDisplayTexture.texture.width - texture.width)*0.5f  + position.x, position.y};    // Camera position
    // camera.target = (Vector2){IMAGEDISPLAY_WIDTH/2.0f, IMAGEDISPLAY_HEIGHT/2.0f};      // Camera looking at point
    camera.target = (Vector2){0, 0};      // Camera looking at point
    camera.zoom = Zoom;

    BeginTextureMode(ImageDisplayTexture);
        BeginMode2D(camera);
            ClearBackground(BLACK);  // Clear render texture background color
            DrawTexture(texture,0,0,WHITE);
        EndMode2D();
    EndTextureMode();

    // return();
}

internal
void AnnotationPage()
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Common Annotation Menu initializations

    Vector2 MousePosition = GetMousePosition();
    // u32 CurrentGesture = GetGestureDetected();
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    
    f32 FullImageDisplayWidth = ScreenWidth > PANELWIDTH ? ScreenWidth - PANELWIDTH : 0;
    f32 FullImageDisplayHeight = ScreenHeight;
    f32 dt = GetFrameTime();
    float scale = MIN(FullImageDisplayWidth/IMAGEDISPLAY_WIDTH, FullImageDisplayHeight/IMAGEDISPLAY_HEIGHT);


    internal u32 CurrentLabel = 0;
    // Rectangle PanelRec = {0,0,PANELWIDTH,(f32)ScreenHeight};

// Inside Image Display Stuff
    const char *ImagePath = TextFormat("%s/%s",TEST_FOLDER_JPG, TEST_PIC);
    // Texture CurrentTexture = LoadTexture(ImagePath);
    
    // s32 count = 0;
    // const char *ImageName = TextSplit(TEST_PIC,'.',&count)[0];
    // const char *AnnPathTmp = TextFormat("%s/%s.ann",PROJECT_FOLDER, ImageName);
    // char* AnnPath = (char*)malloc(strlen(AnnPathTmp) + 1);
    // strcpy(AnnPath,AnnPathTmp);

    disp_mode DisplayMode = DispMode_creation;
    internal f32 Zoom = 1.0f;
    internal Vector2 Position = {0.0f,0.0f};

    if (IsKeyDown(KEY_W))
    {
        Zoom += 1*dt;
    }
    if (IsKeyDown(KEY_S))
    {
        Zoom -= 1*dt;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        Position.x -= 100*dt;
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        Position.x += 100*dt;
    }
    if (IsKeyDown(KEY_UP))
    {
        Position.y -= 100*dt;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        Position.y += 100*dt;
    }
    if (IsKeyPressed(KEY_D))
    {
        DisplayMode = DispMode_manipulation;
    }
    else if (IsKeyPressed(KEY_B))
    {
        DisplayMode = DispMode_creation;
    }

    f32 Ratio;
    internal RenderTexture2D ImageDisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
    SetTextureFilter(ImageDisplayTexture.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use
    if (IsWindowResized())
    {
        UnloadRenderTexture(ImageDisplayTexture);
        ImageDisplayTexture = LoadRenderTexture(FullImageDisplayWidth,FullImageDisplayHeight);
        SetTextureFilter(ImageDisplayTexture.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    }
    ImageDisplay(true,ImagePath,ImageDisplayTexture,Position, Zoom, &Ratio);
    // f32 ImageDisplayHeight = Ratio*FullImageDisplayWidth;
    
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
        DrawFPS(10,10);
    EndDrawing();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}