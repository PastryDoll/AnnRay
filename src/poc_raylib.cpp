/*******************************************************************************************
*
*   raylib [core] example - window scale letterbox (and virtual mouse)
*
*   Example originally created with raylib 2.5, last time updated with raylib 4.0
*
*   Example contributed by Anata (@anatagawa) and reviewed by Ramon Santamaria (@raysan5)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2019-2024 Anata (@anatagawa) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include "raymath.h"        // Required for: Vector2Clamp()

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    const int windowWidth = 800;
    const int windowHeight = 450;

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - window scale letterbox");
    SetWindowMinSize(320, 240);

    int gameScreenWidth = 640;
    int gameScreenHeight = 480;
    Vector2 position = {0.0f,0.0f};
    Camera2D camera = { 0 };
    camera.offset = (Vector2){position.x + gameScreenWidth/2.0f, position.y + gameScreenHeight/2.0f};    // Camera position
    camera.target = (Vector2){gameScreenWidth/2.0f,gameScreenHeight/2.0f};      // Camera looking at point
    camera.rotation = 0.0f;
    camera.zoom = 0.1f;

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    Texture texture =  LoadTexture("/Users/caio/Desktop/Personal/CppTests/AnnRay/test_material/images_sample/DJI_0325.JPG");
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        if (IsKeyDown(KEY_W))
        {
            camera.zoom += 0.01f;
        }
        if (IsKeyDown(KEY_S))
        {
            camera.zoom -= 0.01f;
        }
        if (IsKeyDown(KEY_D))
        {
            position.x += 2.0f;
        }
        if (IsKeyDown(KEY_A))
        {
            position.x -= 2.0f;
        }
        Vector2 mouse = GetMousePosition();
        camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        camera.target = mouse;

        //----------------------------------------------------------------------------------
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);

        // Update virtual mouse (clamped mouse value behind game screen)
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth*scale))*0.5f)/scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight*scale))*0.5f)/scale;
        virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)gameScreenWidth, (float)gameScreenHeight });

        BeginTextureMode(target);
            BeginMode2D(camera);
            ClearBackground(GRAY);  // Clear render texture background color
            DrawTexture(texture,0,0,WHITE);
            // target.texture = texture;
            // for (int i = 0; i < 10; i++) DrawRectangle(0, (gameScreenHeight/10)*i, gameScreenWidth, gameScreenHeight/10, colors[i]);

            // DrawText("If executed inside a window,\nyou can resize the window,\nand see the screen scaling!", 10, 25, 20, BLACK);
            // DrawText(TextFormat("Default Mouse: [%i , %i]", (int)mouse.x, (int)mouse.y), 350, 25, 20, GREEN);
            // DrawText(TextFormat("Virtual Mouse: [%i , %i]", (int)virtualMouse.x, (int)virtualMouse.y), 350, 55, 20, YELLOW);
            EndMode2D();
        EndTextureMode();
        
        BeginDrawing();
            ClearBackground(BLACK);     // Clear screen background

            // Draw render texture to screen, properly scaled
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                           (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
        //--------------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);        // Unload render texture

    CloseWindow();                      // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}