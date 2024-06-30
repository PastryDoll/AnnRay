#define CompletePastBeforeFuture asm volatile("" : : : "memory");

struct thread_info_image
{
    char* FileName;
    bool ImageReady = false;
    dispatch_semaphore_t SemaphoreHandle;
    Image AsyncImage;
};

internal void *AsyncImageLoading(void *arg)
{
    thread_info_image *ThreadInfo = (thread_info_image *)arg;
    for (;;)
    {
        printf("Im a thread\n");
        // if (ThreadInfo->AsyncImage.data != nullptr)
        // {
        //     UnloadImage(ThreadInfo->AsyncImage);
        //     ThreadInfo->AsyncImage.data = nullptr;
        // }
        dispatch_semaphore_wait(ThreadInfo->SemaphoreHandle, DISPATCH_TIME_FOREVER); //decrements semaphore
        CompletePastBeforeFuture
        ThreadInfo->AsyncImage = LoadImage(ThreadInfo->FileName);
        ThreadInfo->ImageReady = true;
    }
    return NULL;
}

internal bool RequestImageAsync(thread_info_image *ThreadInfo, char *FileName)
{
    ThreadInfo->FileName = FileName;
    dispatch_semaphore_signal(ThreadInfo->SemaphoreHandle);
    return ThreadInfo->ImageReady;
}