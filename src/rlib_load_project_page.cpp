internal u32 LoadProjectPage(void)
{
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    f32 Verticalgap = ScreenHeight*0.01f;
    f32 ButtonHeight = ScreenHeight*0.12f;
    f32 ButtonEffectiveHeight = ButtonHeight+Verticalgap;
    f32 ButtonsX = ScreenWidth*1.0f/3.0f;
    FilePathList Projects = LoadDirectoryFiles("../projects");
    u32 DirCount = 0;
    u32 ReturnPage = LOAD_PROJECT_PAGE;
    BeginDrawing();
        ClearBackground(GREEN);
        for (u32 i = 0; i < Projects.count; ++i)
        {
            if (!IsPathFile(Projects.paths[i]))
            {
                const char *DirName = GetFileName(Projects.paths[i]);
                if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + DirCount*ButtonEffectiveHeight, ButtonsX, ButtonHeight},DirName))
                {
                    strcpy(GlobalState.ProjectName, DirName);
                    GlobalState.IsProjectSelected = true;
                    ReturnPage = FRONT_PAGE;
                };
                DirCount++;
            } 
        };
        if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) ReturnPage = FRONT_PAGE;
        DrawFPS(10,10);
    EndDrawing();

    return ReturnPage;

}