enum disp_mode
{
    DispMode_creation,
    DispMode_manipulation,
};

enum box_hit_state
{
    NoHit,
    InsideHit,
    HorizontalHit,
    VerticalHit
};

struct render_display
{
    Camera2D camera;
    Texture texture;
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