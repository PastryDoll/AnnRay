internal const
void SaveLabelsToFile(char Labels[MAX_LENGTH][MAX_STRINGS], u32 TotalLabels)
{
    FILE *file = fopen("../project/label.lann", "w");
    if (file == NULL) {
        printf("Error opening label file!\n");
        assert(0);
    }
    fprintf(file, "%u\n", TotalLabels);
    for (int i = 0; i < MAX_STRINGS; i++) {
        fprintf(file, "%s\n", Labels[i]);
    }
    fclose(file);
}

u32 ReadLabelsFromFile(const char *filename, char Labels[MAX_LENGTH][MAX_STRINGS]) {
    FILE *file = fopen("../project/label.lann", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        assert(0);
    }
    
    u32 TotalLabels;
    fscanf(file, "%u", &TotalLabels);

    if (Labels)
    {
        int i = 0;
        while (i < MAX_STRINGS && (fscanf(file, "%s", Labels[i]) != EOF)) {
            printf("LABELS!!!:%s\n", Labels[i]);
            i++;
        }
        printf("TOTAL LABELS: %u", TotalLabels);
    }
    fclose(file);
    return TotalLabels;
}

internal const
void SaveAnnToFile(const char *FileName, const bboxes *Bboxes)
{
    u32 TotalLabels = ReadLabelsFromFile("../project/label.lann",NULL);
    FILE *file = fopen(FileName, "wb");
    if (file == NULL) {
        printf("Error Saving to file %s\n",FileName);
        fclose(file);
        return;
    }
    fwrite(&(Bboxes->TotalBoxes), sizeof(Bboxes->TotalBoxes),1, file);
    fwrite(&(Bboxes->LabelsCount), sizeof(Bboxes->LabelsCount[0])*TotalLabels, 1, file);
    fwrite(&(Bboxes->Boxes), sizeof(Bboxes->Boxes[0])*Bboxes->TotalBoxes,1, file);
    fclose(file);
}

internal 
void ReadAnnFromFile(const char *FileName, bboxes *NewBboxes)
{

    u32 TotalLabels = ReadLabelsFromFile("../project/label.lann",NULL);
    memset(NewBboxes,0,sizeof(bboxes));

    FILE *file;
    file = fopen(FileName, "rb");
    if (file == NULL) {
        printf("Error opening file %s\n",FileName);
        fclose(file);
        return;
    }

    fread(&NewBboxes->TotalBoxes, sizeof(NewBboxes->TotalBoxes), 1, file);
    fread(&NewBboxes->LabelsCount, sizeof(NewBboxes->LabelsCount[0])*TotalLabels, 1, file);
    fread(&NewBboxes->Boxes, sizeof(NewBboxes->Boxes[0])*NewBboxes->TotalBoxes ,1, file);
    fclose(file);
}