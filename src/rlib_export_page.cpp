#include "yolo_format.h"

internal
u32 ExportPage()
{
    BeginDrawing();
        ClearBackground(GREEN);
        DrawLeftPanel();
        internal s32 Active = 0;
        internal bool Edit = false;
        if(GuiDropdownBox((Rectangle){100,100,200,30},"YOLO(ultralytic);...", &Active,Edit)) Edit = !Edit;
        if(GuiButton((Rectangle){100,140,200,30},"EXPORT")) ConvertProjectToYolo((char*)"test",80,10,10);
        {
            if(GuiButton({PANELWIDTH*0.6,10,PANELWIDTH*0.19,30},"BACK")) return FRONT_PAGE;
        }
        DrawFPS(10,10);
    EndDrawing();

    return EXPORT_PAGE;
};