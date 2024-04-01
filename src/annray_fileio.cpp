internal const
void SaveAnnToFile(const char *FileName, const bboxes *Bboxes)
{
    FILE *file = fopen(FileName, "wb");
    if (file == NULL) {
        printf("Error Saving to file %s\n",FileName);
        fclose(file);
        return;
    }
    printf("%s\n",FileName);

    fwrite(&(Bboxes->TotalBoxes), sizeof(Bboxes->TotalBoxes),1, file);
    fwrite(&(Bboxes->TotalLabels), sizeof(Bboxes->TotalLabels),1, file);
    printf("SAVING TOTAL LABELS %u\n",(Bboxes->TotalLabels));
    fwrite(&(Bboxes->LabelsCount), sizeof(Bboxes->LabelsCount[0])*Bboxes->TotalLabels, 1, file);
    fwrite(&(Bboxes->Boxes), sizeof(Bboxes->Boxes[0])*Bboxes->TotalBoxes,1, file);
    fclose(file);
}

internal 
void ReadAnnFromFile(const char *FileName, bboxes *NewBboxes)
{
    memset(NewBboxes,0,sizeof(bboxes));

    FILE *file;
    file = fopen(FileName, "rb");
    if (file == NULL) {
        printf("Error opening file %s\n",FileName);
        fclose(file);
        return;
    }

    fread(&NewBboxes->TotalBoxes, sizeof(NewBboxes->TotalBoxes), 1, file);
    printf("TOTAL BOXES: %u\n",NewBboxes->TotalBoxes);
    fread(&NewBboxes->TotalLabels, sizeof(NewBboxes->TotalLabels), 1, file);
    printf("TOTAL LABELS: %u\n", NewBboxes->TotalLabels);
    if ( NewBboxes->TotalLabels < 5)  NewBboxes->TotalLabels = 5; //@TODO REMOVE THIS
    printf("TOTAL LABELS: %u\n", NewBboxes->TotalLabels);
    fread(&NewBboxes->LabelsCount, sizeof(NewBboxes->LabelsCount[0])*NewBboxes->TotalLabels, 1, file);
    fread(&NewBboxes->Boxes, sizeof(NewBboxes->Boxes[0])*NewBboxes->TotalBoxes ,1, file);
    fclose(file);
}

internal const
void SaveLabelsToFile(char Labels[MAX_LENGTH][MAX_STRINGS])
{
    FILE *file = fopen("../project/label.lann", "w");
    if (file == NULL) {
        printf("Error opening label file!\n");
        assert(0);
    }

    for (int i = 0; i < MAX_STRINGS; i++) {
        fprintf(file, "%s\n", Labels[i]);
    }
    fclose(file);
}

void ReadLabelsFromFile(const char *filename, char Labels[MAX_LENGTH][MAX_STRINGS]) {
    FILE *file = fopen("../project/label.lann", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        assert(0);
    }

    int i = 0;
    while (i < MAX_STRINGS && (fscanf(file, "%s", Labels[i]) != EOF)) {
        printf("LABELS!!!:%s\n", Labels[i]);
        i++;
    }

    fclose(file);
}