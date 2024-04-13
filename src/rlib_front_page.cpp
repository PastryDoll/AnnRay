internal
u32 FrontPage(Music *FrontPageMuic)
{
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    internal bool MusicOff = false;
    if (!MusicOff) UpdateMusicStream(*FrontPageMuic);
    f32 Verticalgap = ScreenHeight*0.02f;
    f32 ButtonHeight = ScreenHeight*0.15f;
    BeginDrawing();
        ClearBackground(BLUE);
        if(GuiButton((Rectangle){ScreenWidth*1.0f/3.0f,ScreenHeight*0.1f,ScreenWidth*1.0f/3.0f,ButtonHeight},"Annotation Page")) return ANNOTATION_PAGE;
        if(GuiButton((Rectangle){ScreenWidth*1.0f/3.0f,ScreenHeight*0.1f+ButtonHeight+Verticalgap,ScreenWidth*1.0f/3.0f,ButtonHeight},"Inventory Page")) return INVENTORY_PAGE;
        if(GuiButton((Rectangle){ScreenWidth*1.0f/3.0f,ScreenHeight*0.1f+2*(ButtonHeight+Verticalgap),ScreenWidth*1.0f/3.0f,ButtonHeight},"Export Page")) return EXPORT_PAGE;
        if(GuiButton((Rectangle){ScreenWidth*1.0f/3.0f,ScreenHeight*0.75f,ScreenWidth*1.0f/3.0f,ButtonHeight},"Exit")) return EXIT;
        GuiToggle((Rectangle){ScreenWidth*0.97f,ScreenHeight*0.9f,ScreenWidth*0.02f,ScreenWidth*0.02f}, GuiIconText(ICON_AUDIO, ""),&MusicOff);
        DrawFPS(10,10);
    EndDrawing();

    return FRONT_PAGE;
}