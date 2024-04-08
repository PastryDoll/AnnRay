#define internal static
#define global static
#define Assert assert

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s32 b32;
typedef s32 b32x;

typedef float f32;
typedef double f64;

#define U32Max ((u32)-1)
#define F32Max FLT_MAX
#define F32Min -FLT_MAX
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

internal inline
bool IsGestureTapped(s32 Gesture)
{
    if (Gesture & (GESTURE_TAP)) return true;
    return false;
};

internal inline
bool IsGestureDragging(s32 Gesture)
{
    if (Gesture & (GESTURE_DRAG)) return true;
    return false;
};

internal inline
bool IsGestureHolding(s32 Gesture)
{
    if (Gesture & (GESTURE_HOLD)) return true;
    return false;
};

internal inline
bool IsGestureHoldingOrDragging(s32 Gesture)
{
    if (Gesture & (GESTURE_HOLD|GESTURE_DRAG)) return true;
    return false;
};

internal inline
bool IsGestureReleased(s32 CurrGesture, s32 PrevGesture)
{
    if (!(PrevGesture == (GESTURE_NONE)) && 
        (CurrGesture == (GESTURE_NONE))) return true;
    return false;
};

enum pages {

    FRONT_PAGE,
    ANNOTATION_PAGE,
    INVENTORY_PAGE,
    EXIT,
    PAGE_COUNT
};

struct global_state
{
    pages CurrentPage;
    pages PreviousPage;
};
