void avfilter_link_free(AVFilterLink **link)
{
    if (!*link)
        return;
    av_frame_free(&(*link)->partial_buf);
    av_freep(link);
}
