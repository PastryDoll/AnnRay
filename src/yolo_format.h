void ConvertProjectToYolo(char *ProjectName, u32 train, u32 val, u32 test)
{
    // 
    // data.yaml
    // 

    u32 ProjectLen = strlen(ProjectName);
    u32 BaseLen = 12;
    u32 TrainValTestLen = 8;
    char YoloFolder[ProjectLen + BaseLen + TrainValTestLen + 1]; 
    sprintf(YoloFolder, "../project/yolo(ultralytics)-%u-%u-%u",train,val,test);
    s32 result = mkdir(YoloFolder,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if ((result == -1) && errno != EEXIST) fprintf(stderr, "ERROR: Could not create directory %s. %s\n", YoloFolder, strerror(errno));
    else
    {   
        u32 DataBaseLen = strlen(YoloFolder);
        u32 DateFileLen = 9;
        char DateFile[DataBaseLen + DateFileLen + 1];
        sprintf(DateFile, "%s/data.yaml",YoloFolder);
        FILE *file = fopen(DateFile, "wb");
        if (file == NULL) {
            printf("Error Saving to file %s\n",DateFile);
            fclose(file);
        }
        
    }
}