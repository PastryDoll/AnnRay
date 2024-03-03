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

internal 
void ReadInMemoryAnn(const char *FileName, u32 NumBoxes, bbox *NewBoxes)
{
    FILE *file;
    file = fopen(FileName, "rb");
    if (file == NULL) {
        perror("Error opening file");
    }
    bbox Boxes[NumBoxes];
    fread(Boxes, sizeof(bbox), NumBoxes, file);
    for (int i = 0; i < NumBoxes; ++i)
    {
        NewBoxes[i] = Boxes[i];
        printf("File box: %f,%f,%f,%f\n", Boxes[i].Box.x, Boxes[i].Box.y, Boxes[i].Box.height, Boxes[i].Box.width);

    }
    fclose(file);
}
