pages FrontPage(Music *FrontPageMuic)
{
    s32 ScreenHeight = GetScreenHeight();
    s32 ScreenWidth = GetScreenWidth();
    internal bool MusicOff = false;
    if (!MusicOff) UpdateMusicStream(*FrontPageMuic);
    BeginDrawing();
        if(GuiButton((Rectangle){ScreenWidth*1.0f/3.0f,ScreenHeight*0.1f,ScreenWidth*1.0f/3.0f,ScreenHeight*0.15f},"Annotation Page")) return ANNOTATION_PAGE;
        if(GuiButton((Rectangle){ScreenWidth*1.0f/3.0f,ScreenHeight*0.75f,ScreenWidth*1.0f/3.0f,ScreenHeight*0.15f},"Exit")) return EXIT;
        GuiToggle((Rectangle){ScreenWidth*0.97f,ScreenHeight*0.9f,ScreenWidth*0.02f,ScreenWidth*0.02f}, GuiIconText(ICON_AUDIO, ""),&MusicOff);
        ClearBackground(BLUE);
        DrawFPS(10,10);
    EndDrawing();

    return FRONT_PAGE;
}