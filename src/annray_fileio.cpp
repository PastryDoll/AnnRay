internal const
void SaveDataToFile(const char *FileName, bbox *Boxes, u32 NumBoxes)
{
    FILE *file = fopen(FileName, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    printf("%s\n",FileName);

    fwrite(Boxes, sizeof(BoundingBox), NumBoxes, file);

    fclose(file);
}

// internal 
// void ReadInMemoryAnn(const char *FileName, u32 NumBoxes)
// {
//     FILE *file;
//     file = fopen(FileName, "rb");
//     if (file == NULL) {
//         perror("Error opening file");
//     }
//     bbox Boxes[NumBoxes];
//     fread(Boxes, sizeof(bbox), NumBoxes, file);

//     for (u32 BoxId = 0; BoxId < NumBoxes; ++BoxId)
//     {  
//         printf("id: %u,x: %f,y: %f,w: %f,h: %f,label: %u\n", BoxId,Boxes[BoxId].Box.x,Boxes[BoxId].Box.y,Boxes[BoxId].Box.width,Boxes[BoxId].Box.height, Boxes[BoxId].Label);
//     }
//     fclose(file);
// }
