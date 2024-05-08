internal
u32 NewProjectPage(FilePathList PathList)
{
    u32 ScreenWidth = GetScreenWidth();
    u32 FontSize = (u32)ScreenWidth*0.02;
    u32 ProjectNameW = MeasureText("Project Name:",FontSize);
    
    BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Project Name:", ScreenWidth/2 - ProjectNameW,100,FontSize,BLACK);
        DrawFPS(10,10);
    EndDrawing();

    return NEW_PROJECT_PAGE;
};