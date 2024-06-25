enum new_project_text_field
{
    ProjectNameField,
    TagsField
};

u32 CreateProjectFolder(char *ProjectName)
{   
    char *PathStrs[4];
    const char *FullPath;

    // 
    //// Root Folder
    // 
    
    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = ProjectName;
    FullPath = TextJoin((const char **)PathStrs,3,"/");

    if (!DirectoryExists(FullPath))
    {
        CreateDirectory(FullPath);
    }; 

    // 
    //// Annotations Folder
    // 

    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = ProjectName;
    PathStrs[3] = (char *)"annotations";
    FullPath = TextJoin((const char **)PathStrs,4,"/");

    if (!DirectoryExists(FullPath))
    {
        CreateDirectory(FullPath);
    }; 

    // 
    //// Labels Folder
    // 

    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = ProjectName;
    PathStrs[3] = (char *)"labels";
    FullPath = TextJoin((const char **)PathStrs,4,"/");

    if (!DirectoryExists(FullPath))
    {
        CreateDirectory(FullPath);
    }; 

    return 0;
};

internal
u32 NewProjectPage()
{
    u32 CurrentGesture = GetGestureDetected();
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    u32 FontSize = (u32)ScreenWidth*0.02;
    u32 ProjectNameW = MeasureText("Project Name:",FontSize);

    Rectangle ProjectNameRec = {.x = ScreenWidth/2.0f + 10, .y = 100, .width = 300, .height = 30};
    Rectangle TagsRec = {.x = ScreenWidth/2.0f + 10, .y = 100 + 35, .width = 300, .height = 30};
    Rectangle DoneButton = {.x = ScreenWidth*0.8f, .y = ScreenHeight*0.85f, .width = ScreenWidth*0.1f, .height = ScreenHeight*0.05f};

    internal bool ProjectNameActive = false;
    internal bool TagsActive = false;
    internal u32 ProjectNameLetterCount = 0;
    internal u32 TagsLetterCount = 0;

    char Tags[16]= {0};
    
    BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Project Name:", ScreenWidth/2 - ProjectNameW,100,FontSize,BLACK);
        DrawText("Tags:", ScreenWidth/2 - ProjectNameW,100 + 35,FontSize,BLACK);
        TextInputBox(ProjectNameRec, &ProjectNameActive, &CurrentCursorSprite, GlobalState.ProjectName, &ProjectNameLetterCount, MAX_LENGTH);
        TextInputBox(TagsRec, &TagsActive, &CurrentCursorSprite, Tags, &TagsLetterCount, MAX_LENGTH);

        if (CheckCollisionPointRec(GetMousePosition(), ProjectNameRec) && (IsGestureTapped(CurrentGesture)))
        {
            ProjectNameActive = true;
        } 

        DrawText(GlobalState.ProjectName, (u32)ProjectNameRec.x + 5, (u32)ProjectNameRec.y + 5, 20, BLACK);
        if (IsKeyPressed(KEY_ENTER) && ProjectNameActive)
        {
            ProjectNameActive = false;
            TagsActive = true;
        }
        else if (IsKeyPressed(KEY_ENTER) && TagsActive)
        {
            TagsActive = false;
        };

        if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        if(GuiButton(DoneButton,"Done!"))
        {
            CreateProjectFolder(GlobalState.ProjectName);
            return FRONT_PAGE;
        } 
        DrawFPS(10,10);
    EndDrawing();

    return NEW_PROJECT_PAGE;
};