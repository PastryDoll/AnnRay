#define MAX_TOTAL_BOXES 16

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
    HorizontalLeftHit,
    HorizontalRightHit,
    VerticalTopHit,
    VerticalBottomHit,
};

struct segmented_lines
{
    f32 X; 
    f32 Y; 
    f32 W; 
    f32 H;
    Color color;
};

struct annotation_display
{
    Camera2D camera;
    Texture ImageTexture;
    RenderTexture2D DisplayTexture;
};

// struct annotation_page_state_creation
// {
    
// };

// struct annotation_page_state_manipulation
// {
    
// };
struct bbox
{
    u32 Label;
    Rectangle Box;
};
struct bboxes
{
    u32 TotalBoxes;
    bbox Boxes[MAX_TOTAL_BOXES];
};
struct annotation_page_state
{   
    u32 DisplayMode;
    s32 CurrentGesture;
    s32 PrevGesture;
    s32 CurrentLabel;
    u32 CurrentBbox;
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