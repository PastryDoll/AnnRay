global char ProjectName[MAX_LENGTH] = "\0";      // NOTE: One extra space required for null terminator char '\0'
global bool IsProjectNameSet = false;

internal 
u32 CreateProjectFolder()
{

};

internal
u32 NewProjectPage(FilePathList PathList)
{
    u32 ScreenWidth = GetScreenWidth();
    u32 FontSize = (u32)ScreenWidth*0.02;
    u32 ProjectNameW = MeasureText("Project Name:",FontSize);
    Rectangle ProjectNameRec = {.x = ScreenWidth/2.0f + 10, .y = 100, .width = 300, .height = 30};
    internal bool Active = true;
    internal u32 LetterCount = 0;
    
    BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Project Name:", ScreenWidth/2 - ProjectNameW,100,FontSize,BLACK);
        TextInputBox(ProjectNameRec, &Active, &CurrentCursorSprite, ProjectName, &LetterCount, MAX_LENGTH);
        DrawText(ProjectName, (u32)ProjectNameRec.x + 5, (u32)ProjectNameRec.y + 5, 20, BLACK);
        if (IsKeyPressed(KEY_ENTER))
        {
            Active = false;
            IsProjectNameSet = true;
            CreateProjectFolder();
        } 
        if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        DrawFPS(10,10);
    EndDrawing();

    return NEW_PROJECT_PAGE;
};