static int opt_progress(void *optctx, const char *opt, const char *arg)
{
    AVIOContext *avio = NULL;
    int ret;
    if (!strcmp(arg, "-"))
        arg = "pipe:";
    ret = avio_open2(&avio, arg, AVIO_FLAG_WRITE, &int_cb, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Failed to open progress URL \"%s\": %s\n",
               arg, av_err2str(ret));
        return ret;
    }
    progress_avio = avio;
    return 0;
}
