}
static int alloc_init( filter_t *p_filter, void *p_data )
{
    p_filter->p_owner = p_data;
    p_filter->pf_video_buffer_new = video_new;
    p_filter->pf_video_buffer_del = video_del;
    return VLC_SUCCESS;
}
