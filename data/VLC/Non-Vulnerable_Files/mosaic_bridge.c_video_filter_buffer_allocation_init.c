}
static int video_filter_buffer_allocation_init( filter_t *p_filter, void *p_data )
{
    p_filter->pf_video_buffer_new = video_new_buffer_filter;
    p_filter->pf_video_buffer_del = video_del_buffer_filter;
    p_filter->p_owner = p_data;
    return VLC_SUCCESS;
}
