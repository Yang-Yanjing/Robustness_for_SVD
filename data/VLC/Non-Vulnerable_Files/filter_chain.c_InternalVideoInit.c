}
static int InternalVideoInit( filter_t *p_filter, void *p_data )
{
    VLC_UNUSED(p_data);
    p_filter->pf_video_buffer_new = VideoBufferNew;
    p_filter->pf_video_buffer_del = VideoBufferDelete;
    return VLC_SUCCESS;
}
