static void ffprobe_cleanup(int ret)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(sections); i++)
        av_dict_free(&(sections[i].entries_to_show));
#if HAVE_THREADS
    pthread_mutex_destroy(&log_mutex);
#endif
}
