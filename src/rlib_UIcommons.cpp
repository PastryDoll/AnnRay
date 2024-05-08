char name[MAX_LENGTH + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
int letterCount = 0;

//@TODO block other shortkeys
internal 
void TextInputBox(Rectangle rectangle, bool *Active, u8 *CurrentCursorSprite)
{
    if (CheckCollisionPointRec(GetMousePosition(), rectangle))
    {
        if (*Active) *CurrentCursorSprite = MOUSE_CURSOR_IBEAM;
        int key = GetCharPressed();
        while (key > 0)
        {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (letterCount < MAX_LENGTH))
            {
                name[letterCount] = (char)key;
                name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }

            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            name[letterCount] = '\0';
        }

    }
    DrawRectangleRec(rectangle, BLUE);
    // GuiDrawText(name, GetTextBounds(TOGGLE, rectangle), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), GetColor(GuiGetStyle(TOGGLE, TEXT + STATE_NORMAL*3)));
    DrawText(name, (u32)rectangle.x + 5, (u32)rectangle.y + 5, 20, BLACK);
    if ((letterCount < MAX_LENGTH) && (*Active))
    {

        // Draw blinking underscore char
        if ((((u32)(GetTime()*2))%2) == 0) DrawText("|", (int)rectangle.x + 8 + MeasureText(name, 20), (u32)rectangle.y + 5, 20, BLACK);
    }
}