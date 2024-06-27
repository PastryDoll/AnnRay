enum new_project_text_field
{
    ProjectNameField,
    ImageFolderField,
    TagsField,
    FieldsCount
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

    Rectangle FieldsRecs[FieldsCount];
    Rectangle ProjectNameRec = {.x = ScreenWidth/2.0f + 10, .y = 100, .width = 300, .height = 30};
    Rectangle ImageFolderRec = {.x = ScreenWidth/2.0f + 10, .y = 100 + 35, .width = 300, .height = 30};
    Rectangle TagsRec = {.x = ScreenWidth/2.0f + 10, .y = 100 + 2*35, .width = 300, .height = 30};
    FieldsRecs[ProjectNameField] = ProjectNameRec; 
    FieldsRecs[ImageFolderField] = ImageFolderRec; 
    FieldsRecs[TagsField] = TagsRec; 

    internal bool FieldsActive[FieldsCount] = {0};

    char *FieldsTmpTextPtrs[FieldsCount];
    internal char TmpProjectName[MAX_LENGTH]; 
    internal char TmpImageFolder[512]; 
    internal char TmpTags[512]; 
    FieldsTmpTextPtrs[ImageFolderField] = TmpImageFolder;
    FieldsTmpTextPtrs[ProjectNameField] = TmpProjectName;
    FieldsTmpTextPtrs[TagsField] = TmpTags;

    internal u32 LetterCount[FieldsCount];
    u32 MaxLengths[FieldsCount] = {MAX_LENGTH, 512, MAX_LENGTH};

    Rectangle DoneButton = {.x = ScreenWidth*0.8f, .y = ScreenHeight*0.85f, .width = ScreenWidth*0.1f, .height = ScreenHeight*0.05f};
    
    BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Project Name:", ScreenWidth/2 - ProjectNameW,100,FontSize,BLACK);
        DrawText("Images Folder", ScreenWidth/2 - ProjectNameW,100 + 35,FontSize,BLACK);
        DrawText("Tags:", ScreenWidth/2 - ProjectNameW,100 + 2*35,FontSize,BLACK);


        for (u32 i = 0; i < FieldsCount; ++i)
        {
            TextInputBox(FieldsRecs[i], &FieldsActive[i], &CurrentCursorSprite, FieldsTmpTextPtrs[i], &LetterCount[i], MaxLengths[i]);
            DrawText(FieldsTmpTextPtrs[i], (u32)FieldsRecs[i].x + 5, (u32)FieldsRecs[i].y + 5, 20, BLACK);

            if (CheckCollisionPointRec(GetMousePosition(), FieldsRecs[i]) && (IsGestureTapped(CurrentGesture)))
            {
                for (u32 j = 0; j < FieldsCount; ++j)
                {
                    FieldsActive[j] = (j == i);
                }
            } 
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            for (u32 i = 0; i < FieldsCount; ++i)
            {
                if(FieldsActive[i])
                {
                    FieldsActive[i] = false;
                    printf("i: %u\n", i);

                    if (i + 1 < FieldsCount) FieldsActive[i+1] = true;
                    break;
                }
            }
        }

        {
            bool NonEmptyTexts = true;
            for (u32 i = 0; i < FieldsCount; ++i)
            {
                if (!(TextLength(FieldsTmpTextPtrs[i]) > 0))
                {
                    NonEmptyTexts = false;
                    break;
                }
            }
            if (NonEmptyTexts)
            {
                CreateProjectFolder(GlobalState.ProjectName);
                TextCopy(GlobalState.ProjectName, TmpProjectName);
                return FRONT_PAGE;
            }
        } 

        if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        if(GuiButton(DoneButton,"Done!"))

        DrawFPS(10,10);
    EndDrawing();

    return NEW_PROJECT_PAGE;
};