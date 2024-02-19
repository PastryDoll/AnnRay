enum disp_mode
{
    DispMode_creation,
    DispMode_manipulation,
    DispMode_moving
};

enum box_hit_state
{
    NoHit,
    InsideHit,
    HorizontalHit,
    VerticalHit
};

struct annotation_display
{
    Camera2D camera;
    Texture ImageTexture;
    RenderTexture2D DisplayTexture;
};

struct annotation_page_state
{   
    u32 CurrentLabel;
    u32 CurrentBbox;
    u32 TotalBbox; 
    disp_mode DisplayMode;
};

struct bbox
{
    u32 Label;
    Rectangle Box;
    bool Selected;
};

struct zoom
{
    f32 Strenght;
    Vector2 Position;
};

struct LoadedTexture
{
    Texture CurrentTexture;
    char *Path;    
};