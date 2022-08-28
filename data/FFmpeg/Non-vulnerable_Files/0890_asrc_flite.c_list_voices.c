static void list_voices(void *log_ctx, const char *sep)
{
    int i, n = FF_ARRAY_ELEMS(voice_entries);
    for (i = 0; i < n; i++)
        av_log(log_ctx, AV_LOG_INFO, "%s%s",
               voice_entries[i].name, i < (n-1) ? sep : "\n");
}
