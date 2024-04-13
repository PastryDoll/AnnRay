internal
u32 ExportPage(FilePathList PathList)
{
    BeginDrawing();
        ClearBackground(GREEN);
        DrawLeftPanel();
        {
            if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        }
        DrawFPS(10,10);
    EndDrawing();

    return EXPORT_PAGE;
};