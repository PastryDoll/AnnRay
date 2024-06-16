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

#define MAX_TOTAL_BOXES 16

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

u32 CreateDirectory(const char *dirName) {
    if (dirName == NULL) {
        fprintf(stderr, "Directory name is NULL.\n");
        return 0;
    }
    if (MKDIR(dirName) == 0) {
        printf("Directory '%s' created successfully.\n", dirName);
        return 1;
    } else {
        printf("Error creating directory '%s'.\n", dirName);
        return 0;
    }
}

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
    NEW_PROJECT_PAGE,
    LOAD_PROJECT_PAGE,
    ANNOTATION_PAGE,
    INVENTORY_PAGE,
    EXPORT_PAGE,
    EXIT,
    PAGE_COUNT
};

struct global_state
{
    u32 CurrentPage;
    u32 PreviousPage;
};

struct bbox
{
    u32 Label;
    Rectangle Box;
};

struct __attribute__((packed)) bboxes
{
    u32 TotalBoxes;
    u32 LabelsCount[MAX_STRINGS];
    bbox Boxes[MAX_TOTAL_BOXES];
};