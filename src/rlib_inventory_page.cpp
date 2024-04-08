// internal
// void GenerateThumbnails(Image PreviewImages[], Texture PreviewTextures[], FilePathList PathList)
// {   
//     for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
//         {
//             char *Path = *(PathList.paths + PathIndex);
//             PreviewImages[PathIndex] = LoadImage(Path);
//             ImageResize(&PreviewImages[PathIndex],128,128);
//             PreviewTextures[PathIndex] = LoadTextureFromImage(PreviewImages[PathIndex]);
//             UnloadImage(PreviewImages[PathIndex]);
//         }
//     return;
// }

// internal
// void DrawThumnails(Texture PreviewTextures[], FilePathList PathList)
// {
//     for (u32 PathIndex = 0; PathIndex < PathList.count; ++PathIndex)
//         {   
//             Texture *texture = PreviewTextures + PathIndex;
//             DrawTexturePro(*texture, (Rectangle){0,0,128,128},(Rectangle){0 + 110.f*PathIndex,0,100,100},(Vector2){0,0},0,WHITE);
//         }
// }

// #if TESTTHUMB
//             DrawThumnails(PreviewTextures,PathList);
// #endif