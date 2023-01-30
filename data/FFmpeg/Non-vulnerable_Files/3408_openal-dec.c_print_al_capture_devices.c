static inline void print_al_capture_devices(void *log_ctx)
{
    const char *devices;
    if (!(devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER)))
        return;
    av_log(log_ctx, AV_LOG_INFO, "List of OpenAL capture devices on this system:\n");
    for (; *devices != '\0'; devices += strlen(devices) + 1)
        av_log(log_ctx, AV_LOG_INFO, "  %s\n", devices);
}
