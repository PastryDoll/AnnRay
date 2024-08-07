internal
u32 FrontPage(Music *FrontPageMuic)
{
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    internal bool MusicOff = true;
    if (!MusicOff) UpdateMusicStream(*FrontPageMuic);
    f32 Verticalgap = ScreenHeight*0.01f;
    f32 ButtonHeight = ScreenHeight*0.12f;
    f32 ButtonEffectiveHeight = ButtonHeight+Verticalgap;
    f32 ButtonsX = ScreenWidth*1.0f/3.0f;
    u32 ReturnPage = FRONT_PAGE;
    internal bool DrawError = false;
    BeginDrawing();
        if(GlobalState.IsProjectSelected) DrawError = false;
        else DrawError = true;
        ClearBackground(BLUE);
        char *CurrentProjectTextStrs[] = {(char*)"Current project: ", GlobalState.ProjectName};
        const char *CurrentProjectText =  TextJoin((const char**)CurrentProjectTextStrs, 2, "");
        DrawText(CurrentProjectText, ButtonsX, ScreenHeight*0.05, 20,BLACK);
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 0*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"New Project")) ReturnPage = NEW_PROJECT_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 1*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Load Project")) ReturnPage = LOAD_PROJECT_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 2*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Annotation Page"))
        {
            if(GlobalState.IsProjectSelected) ReturnPage = ANNOTATION_PAGE;
        }
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 3*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Inventory Page"))
        {
            if(GlobalState.IsProjectSelected) ReturnPage = INVENTORY_PAGE;
        }
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 4*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Export Page")) ReturnPage = EXPORT_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 5*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Exit")) ReturnPage = EXIT;
        if(DrawError) DrawText("Please select a project!", ButtonsX + ButtonsX + 10, ScreenHeight*0.1f + 2*ButtonEffectiveHeight + 0.5*ButtonHeight - 30*0.5, 30, RED);
        if(DrawError) DrawText("Please select a project!", ButtonsX + ButtonsX + 10, ScreenHeight*0.1f + 3*ButtonEffectiveHeight + 0.5*ButtonHeight - 30*0.5, 30, RED);
        GuiToggle((Rectangle){ScreenWidth*0.97f,ScreenHeight*0.9f,ScreenWidth*0.02f,ScreenWidth*0.02f}, GuiIconText(ICON_AUDIO, ""),&MusicOff);
        DrawFPS(10,10);
    EndDrawing();

    return ReturnPage;
}