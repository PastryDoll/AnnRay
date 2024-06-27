internal const
void SaveLabelsToFile(char Labels[MAX_LENGTH][MAX_STRINGS], u32 TotalLabels)
{
    char *PathStrs[5];
    const char *FullPath;
    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = GlobalState.ProjectName;
    PathStrs[3] = (char *)"labels";
    PathStrs[4] = (char *)"label.lann";
    FullPath = TextJoin((const char **)PathStrs,5,"/");

    FILE *file = fopen(FullPath, "w");
    if (file == NULL) {
        printf("Error opening label file!\n");
        assert(0);
    }
    fprintf(file, "%u\n", TotalLabels);
    for (int i = 0; i < TotalLabels-1; i++) {
        fprintf(file, "%s\n", Labels[i]);
    }
    fprintf(file, "%s", Labels[TotalLabels-1]);

    fclose(file);
}

u32 ReadLabelsFromFile(char Labels[MAX_LENGTH][MAX_STRINGS]) 
{
    char *PathStrs[5];
    const char *FullPath;
    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = GlobalState.ProjectName;
    PathStrs[3] = (char *)"labels";
    PathStrs[4] = (char *)"label.lann";
    FullPath = TextJoin((const char **)PathStrs,5,"/");
    FILE *file = fopen(FullPath, "r");
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
            i++;
        }
    }
    fclose(file);
    return TotalLabels;
}

internal const
u32 SaveAnnToFile(char *FileName, const bboxes *Bboxes)
{
    u32 TotalLabels = ReadLabelsFromFile(NULL); //This modifies the FullPath *

    char *PathStrs[5];
    const char *FullPath;
    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = GlobalState.ProjectName;
    PathStrs[3] = (char *)"annotations";
    PathStrs[4] = FileName;
    FullPath = TextJoin((const char **)PathStrs,5,"/");
    FILE *file = fopen(FullPath, "wb");
    if (file == NULL) {
        printf("Error Saving to file %s\n",FullPath);
        fclose(file);
        return 0;
    }
    fwrite(&(Bboxes->TotalBoxes), sizeof(Bboxes->TotalBoxes),1, file);
    fwrite(&TotalLabels, sizeof(TotalLabels),1, file);
    fwrite(&(Bboxes->LabelsCount), sizeof(Bboxes->LabelsCount[0])*TotalLabels, 1, file);
    fwrite(&(Bboxes->Boxes), sizeof(Bboxes->Boxes[0])*Bboxes->TotalBoxes,1, file);
    fclose(file);
    return TotalLabels;
}

internal 
u32 ReadAnnFromFile(char *FileName, bboxes *NewBboxes)
{
    char *PathStrs[5];
    const char *FullPath;
    PathStrs[0] = (char *)"..";
    PathStrs[1] = (char *)"projects";
    PathStrs[2] = GlobalState.ProjectName;
    PathStrs[3] = (char *)"annotations";
    PathStrs[4] = FileName;
    FullPath = TextJoin((const char **)PathStrs,5,"/");

    // u32 TotalLabels = ReadLabelsFromFile("../project/label.lann",NULL);
    u32 TotalLabels; 
    memset(NewBboxes,0,sizeof(bboxes));

    FILE *file;
    file = fopen(FullPath, "rb");
    if (file == NULL) {
        printf("Error opening file %s\n",FullPath);
        fclose(file);
        return 1;
    }

    fread(&NewBboxes->TotalBoxes, sizeof(NewBboxes->TotalBoxes), 1, file);
    fread(&TotalLabels, sizeof(TotalLabels), 1, file);
    fread(&NewBboxes->LabelsCount, sizeof(NewBboxes->LabelsCount[0])*TotalLabels, 1, file);
    fread(&NewBboxes->Boxes, sizeof(NewBboxes->Boxes[0])*NewBboxes->TotalBoxes ,1, file);
    fclose(file);
    return TotalLabels;
}