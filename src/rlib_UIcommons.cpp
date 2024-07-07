//@TODO block other shortkeys
internal 
void TextInputBox(Rectangle rectangle, bool *Active, u8 *CurrentCursorSprite, char* name, u32 *letterCount, u32 maxLenght, u32 fontSize)
{
    DrawRectangleRec(rectangle, BLUE);
    if (*Active)
    {
        *CurrentCursorSprite = MOUSE_CURSOR_IBEAM;
        int key = GetCharPressed();
        while (key > 0)
        {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (*letterCount < maxLenght - 1))
            {
                name[*letterCount] = (char)key;
                name[*letterCount+1] = '\0'; // Add null terminator at the end of the string.
                (*letterCount)++;
            }

            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {   
            if (*letterCount > 0) (*letterCount)--;
            name[*letterCount] = '\0';
        }
        // GuiDrawText(name, GetTextBounds(TOGGLE, rectangle), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), GetColor(GuiGetStyle(TOGGLE, TEXT + STATE_NORMAL*3)));
        if (*letterCount < maxLenght)
        {

            // Draw blinking underscore char
            if ((((u32)(GetTime()*2))%2) == 0) DrawText("|", (int)rectangle.x + 8 + MeasureText(name, fontSize), (u32)rectangle.y + 5, fontSize, BLACK);
        }
        else 
        {
            *Active = false;
        }
    } 
}