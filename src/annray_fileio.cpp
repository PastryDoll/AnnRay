internal const
void SaveAnnToFile(const char *FileName, const bboxes *Bboxes)
{
    FILE *file = fopen(FileName, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    printf("%s\n",FileName);

    fwrite(Bboxes, sizeof(Bboxes->TotalBoxes),1, file);
    fwrite(Bboxes, sizeof(Bboxes->TotalLabels),1, file);
    printf("SAVING TOTAL LABELS %u\n",(Bboxes->TotalLabels));
    fwrite(Bboxes, sizeof(Bboxes->TotalLabels)*Bboxes->TotalLabels, 1, file);
    fwrite(Bboxes, sizeof(bbox)*Bboxes->TotalBoxes,1, file);
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
        file = fopen(FileName, "wb");
        if (file == NULL) {
            printf("Error creating file %s\n", FileName);
            assert(0);
        }
        fwrite(NewBboxes,sizeof(bboxes),1,file);
        fclose(file);
    }

    file = fopen(FileName, "rb");
    if (file == NULL) {
        printf("Error opening file %s\n",FileName);
        exit(0);
    }

    fread(&NewBboxes->TotalBoxes, sizeof(NewBboxes->TotalBoxes), 1, file);
    fread(&NewBboxes->TotalBoxes, sizeof(NewBboxes->TotalLabels), 1, file);
    printf("TOTAL LABELS: %u\n", NewBboxes->TotalLabels);
    if ( NewBboxes->TotalLabels < 5)  NewBboxes->TotalLabels = 5; //@TODO REMOVE THIS
    printf("TOTAL LABELS: %u\n", NewBboxes->TotalLabels);
    fread(&NewBboxes->LabelsCount, sizeof(u32), NewBboxes->TotalLabels, file);
    fread(&NewBboxes->TotalBoxes, sizeof(bbox), NewBboxes->TotalBoxes, file);
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