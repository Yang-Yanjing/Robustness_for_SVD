static void sub2video_flush(InputStream *ist)
{
    int i;
    if (ist->sub2video.end_pts < INT64_MAX)
        sub2video_update(ist, NULL);
    for (i = 0; i < ist->nb_filters; i++)
        av_buffersrc_add_frame(ist->filters[i]->filter, NULL);
}
