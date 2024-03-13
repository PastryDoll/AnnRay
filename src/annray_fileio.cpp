internal const
void SaveDataToFile(const char *FileName, const bboxes *Bboxes)
{
    FILE *file = fopen(FileName, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    printf("%s\n",FileName);

    fwrite(Bboxes, sizeof(Bboxes->TotalBoxes) + Bboxes->TotalBoxes*sizeof(bbox), Bboxes->TotalBoxes, file);

    fclose(file);
}

internal 
void ReadInMemoryAnn(const char *FileName, bboxes *NewBboxes)
{
    FILE *file;
    file = fopen(FileName, "rb");
    if (file == NULL) {
        perror("Error opening file");
    }
    u32 TotalBoxes;
    fread(&TotalBoxes, sizeof(NewBboxes->TotalBoxes), 1, file);
    NewBboxes->TotalBoxes = TotalBoxes;
    fread(&NewBboxes->TotalBoxes, sizeof(bbox), TotalBoxes, file);
    // for (int i = 0; i < TotalBoxes; ++i)
    // {
    //     NewBboxes->Boxes[i] = Boxes[i];
    //     printf("File box: %f,%f,%f,%f\n", Boxes[i].Box.x, Boxes[i].Box.y, Boxes[i].Box.height, Boxes[i].Box.width);

    // }
    fclose(file);
}
