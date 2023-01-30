}
static void InternalVideoClean( filter_t *p_filter )
{
    p_filter->pf_video_buffer_new = NULL;
    p_filter->pf_video_buffer_del = NULL;
}
