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
    BeginDrawing();
        ClearBackground(BLUE);
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 0*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"New Project")) return NEW_PROJECT_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 1*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Annotation Page")) return ANNOTATION_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 2*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Inventory Page")) return INVENTORY_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 3*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Export Page")) return EXPORT_PAGE;
        if(GuiButton((Rectangle){ ButtonsX, ScreenHeight*0.1f + 4*ButtonEffectiveHeight, ButtonsX, ButtonHeight},"Exit")) return EXIT;
        GuiToggle((Rectangle){ScreenWidth*0.97f,ScreenHeight*0.9f,ScreenWidth*0.02f,ScreenWidth*0.02f}, GuiIconText(ICON_AUDIO, ""),&MusicOff);
        DrawFPS(10,10);
    EndDrawing();

    return FRONT_PAGE;
}