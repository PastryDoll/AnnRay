
#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

enum new_project_text_field
{
    ProjectNameField,
    ImageFolderField,
    TagsField,
    FieldsCount
};

u32 CopyImagesToFolder(char* dst, FilePathList src)
{
    char buffer[8*1024];
    u32 n;
    for (u32 i = 0; i < src.count; ++i)
    {
        char *SrcPath = src.paths[i];
        const char *DstPath = TextFormat("%s/%s", dst, GetFileName(SrcPath));

        FILE *SrcFile = fopen(SrcPath, "rb");
        if (SrcFile == NULL) {
            perror("Error opening source file");
            return -1;
        }

        FILE *DstFile = fopen(DstPath, "wb");
        if (DstFile == NULL) {
            perror("Error opening destination file");
            fclose(SrcFile);
            return -1;
        }

        while ((n = fread(buffer, sizeof(char), sizeof(buffer), SrcFile)) > 0) {
            if (fwrite(buffer, sizeof(char), n, DstFile) != n) {
                perror("Error writing to destination file");
                fclose(SrcFile);
                fclose(DstFile);
                return -1;
            }
        }
        
        fclose(SrcFile);
        fclose(DstFile);
    }
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
    Rectangle TagsRec = {.x = ScreenWidth/2.0f + 10, .y = 100 + 2*35, .width = 300, .height = 30};
    FieldsRecs[ProjectNameField] = ProjectNameRec; 
    FieldsRecs[ImageFolderField] = ImageFolderRec; 
    FieldsRecs[TagsField] = TagsRec; 

    internal bool FieldsActive[FieldsCount] = {0};

    char *FieldsTmpTextPtrs[FieldsCount];
    internal char TmpProjectName[MAX_LENGTH]; 
    internal char TmpImageFolder[512]; 
    internal char TmpTags[512]; 
    FieldsTmpTextPtrs[ProjectNameField] = TmpProjectName;
    FieldsTmpTextPtrs[ImageFolderField] = TmpImageFolder;
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
            TextInputBox(FieldsRecs[i], &FieldsActive[i], &CurrentCursorSprite, FieldsTmpTextPtrs[i], &LetterCount[i], MaxLengths[i],2);
            // printf("Letter count %u\n", LetterCount[i]);
            // printf("Text: %s\n", FieldsTmpTextPtrs[i]);
            // printf("Active: %u\n", FieldsActive[i]);
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
                TextCopy(GlobalState.ProjectName, TmpProjectName);
                CreateProjectFolder(GlobalState.ProjectName);
                FilePathList ImagesPaths = LoadDirectoryFiles(FieldsTmpTextPtrs[ImageFolderField]);
                CopyImagesToFolder((char*)TextFormat("../projects/%s/images", GlobalState.ProjectName),ImagesPaths);
                GlobalState.IsProjectSelected = true;
                return FRONT_PAGE;
            }
        }
        DrawFPS(10,10);
    EndDrawing();

    return NEW_PROJECT_PAGE;
};