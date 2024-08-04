#include "yolo_format.h"

internal
u32 ExportPage()
{
    u32 ScreenWidth = GetScreenWidth();
    u32 ScreenHeight = GetScreenHeight();
    f32 FormatsButtonWidth = 200;
    BeginDrawing();
        ClearBackground(GREEN);
        DrawLeftPanel();
        internal s32 Active = 0;
        internal bool Edit = false;
        if(!Edit)
        {
            if(GuiButton((Rectangle){PANELWIDTH + (ScreenWidth - PANELWIDTH - FormatsButtonWidth)*0.5f,140,FormatsButtonWidth,30},"EXPORT")) ConvertProjectToYolo((char*)"test",80,10,10);
            {
                if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
            }
        }
        if(GuiDropdownBox((Rectangle){PANELWIDTH + (ScreenWidth - PANELWIDTH - FormatsButtonWidth)*0.5f ,100,FormatsButtonWidth,30},"YOLO(ultralytic);...", &Active,Edit)) Edit = !Edit;
        DrawFPS(10,10);
    EndDrawing();

    return EXPORT_PAGE;
};