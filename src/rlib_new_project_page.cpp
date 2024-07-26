
#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

enum new_project_text_field
{
    ProjectNameField,
    ImageFolderField,
    LabelsField,
    FieldsCount
};

u32 GetLabels(char *Labels)
{   
    s32 count; 
    const char **SplitLabels;
    SplitLabels = TextSplit(Labels, ';', &count);
    SaveLabelsToFile(SplitLabels, count);
    return 1;
}

u32 CopyImageToFolder(char* dst, FilePathList src, u32 CurrIndex)
{
    char buffer[8*1024];
    u32 n;
    char *SrcPath = src.paths[CurrIndex];
    const char *DstPath = TextFormat("%s/%s", dst, GetFileName(SrcPath));
    printf("Copyng : %s to %s with %s\n", SrcPath, DstPath, dst);

    FILE *SrcFile = fopen(SrcPath, "rb");
    if (SrcFile == NULL) {
        perror("Error opening source file");
        return 0;
    }

    FILE *DstFile = fopen(DstPath, "wb");
    if (DstFile == NULL) {
        perror("Error opening destination file");
        fclose(SrcFile);
        return 0;
    }

    while ((n = fread(buffer, sizeof(char), sizeof(buffer), SrcFile)) > 0) {
        if (fwrite(buffer, sizeof(char), n, DstFile) != n) {
            perror("Error writing to destination file");
            fclose(SrcFile);
            fclose(DstFile);
            return 0;
        }
    }
    
    fclose(SrcFile);
    fclose(DstFile);
    return 1;
}

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

    // 
    //// Images Folder
    //  

    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = ProjectName;
    PathStrs[3] = (char *)"images";
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
    internal GuiWindowFileDialogState fileDialogState;
    if (GlobalState.PreviousPage != NEW_PROJECT_PAGE)
    {
        fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    }

    u32 CurrentGesture = GetGestureDetected();
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    u32 FontSize = (u32)ScreenWidth*0.02;
    u32 ProjectNameW = MeasureText("Project Name:",FontSize);

    Rectangle FieldsRecs[FieldsCount];
    Rectangle ProjectNameRec = {.x = ScreenWidth/2.0f + 10, .y = 100, .width = 300, .height = 30};
    Rectangle ImageFolderRec = {.x = ScreenWidth/2.0f + 10, .y = 100 + 35, .width = 300, .height = 30};
    Rectangle LabelsRec = {.x = ScreenWidth/2.0f + 10, .y = 100 + 2*35, .width = 300, .height = 30};
    FieldsRecs[ProjectNameField] = ProjectNameRec; 
    FieldsRecs[ImageFolderField] = ImageFolderRec; 
    FieldsRecs[LabelsField] = LabelsRec; 

    internal bool FieldsActive[FieldsCount] = {0};

    char *FieldsTmpTextPtrs[FieldsCount];
    internal char TmpProjectName[MAX_LENGTH]; 
    internal char TmpImageFolder[512]; 
    internal char TmpLabels[512]; 
    FieldsTmpTextPtrs[ProjectNameField] = TmpProjectName;
    FieldsTmpTextPtrs[ImageFolderField] = TmpImageFolder;
    FieldsTmpTextPtrs[LabelsField] = TmpLabels;

    internal u32 LetterCount[FieldsCount];
    u32 MaxLengths[FieldsCount] = {MAX_LENGTH, 512, MAX_LENGTH};

    Rectangle DoneButton = {.x = ScreenWidth*0.8f, .y = ScreenHeight*0.85f, .width = ScreenWidth*0.1f, .height = ScreenHeight*0.05f};

    internal bool Processing = false;
    internal bool Done = false;
    
    BeginDrawing();
        ClearBackground(GREEN);
        DrawText("Project Name:", ScreenWidth/2 - ProjectNameW,100,FontSize,BLACK);
        DrawText("Images Folder", ScreenWidth/2 - ProjectNameW,100 + 35,FontSize,BLACK);
        DrawText("Labels (separeted by ;):", ScreenWidth/2 - ProjectNameW,100 + 2*35,FontSize,BLACK);


        for (u32 i = 0; i < FieldsCount; ++i)
        {
            TextInputBox(FieldsRecs[i], &FieldsActive[i], &CurrentCursorSprite, FieldsTmpTextPtrs[i], &LetterCount[i], MaxLengths[i],2);
            DrawText(FieldsTmpTextPtrs[i], (u32)FieldsRecs[i].x + 5, (u32)FieldsRecs[i].y + 5, 2, BLACK);

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
                    if (i + 1 < FieldsCount) FieldsActive[i+1] = true;
                    break;
                }
            }
        }

        if (fileDialogState.windowActive) GuiLock();
        if (GuiButton((Rectangle){ ImageFolderRec.x + ImageFolderRec.width, ImageFolderRec.y, ImageFolderRec.height, ImageFolderRec.height }, GuiIconText(ICON_ARROW_DOWN, ""))) fileDialogState.windowActive = true;
        GuiUnlock();
        GuiWindowFileDialog(&fileDialogState);
        if (fileDialogState.SelectFilePressed)
        {
            fileDialogState.SelectFilePressed = false;
            char *PathSelected = fileDialogState.dirPathText;
            if (!IsPathFile(PathSelected))
            {
                TextCopy(FieldsTmpTextPtrs[ImageFolderField], PathSelected);
            }
            else
            {
                fileDialogState.windowActive = true;
            } 

        } 
        if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;

        internal char ImgsFolder[1024];
        internal FilePathList ImagesPaths;
        internal u32 CurrIndexCopy = 0;
        if(GuiButton(DoneButton,"Done!"))
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
                Processing = true;
                TextCopy(GlobalState.ProjectName, TmpProjectName);
                CreateProjectFolder(GlobalState.ProjectName);
                ImagesPaths = LoadDirectoryFiles(FieldsTmpTextPtrs[ImageFolderField]);
                TextCopy(ImgsFolder, TextFormat("../projects/%s/images",GlobalState.ProjectName));
                GlobalState.IsProjectSelected = true;
                PathList = LoadDirectoryFiles(TextFormat("../projects/%s/images", GlobalState.ProjectName));
                GetLabels(FieldsTmpTextPtrs[LabelsField]);
            }
        }
        if (Processing)
        {
            CopyImageToFolder(ImgsFolder,ImagesPaths, CurrIndexCopy++);
            if (CurrIndexCopy >= ImagesPaths.count)
            {
                CurrIndexCopy = 0;
                Done = true;
            } 
            else 
            {
                DrawText(TextFormat("%u/%u", CurrIndexCopy, ImagesPaths.count), 100,100,50,RED);
            }
        }
        DrawFPS(10,10);
    EndDrawing();
    if (Done) return FRONT_PAGE;
    return NEW_PROJECT_PAGE;
};