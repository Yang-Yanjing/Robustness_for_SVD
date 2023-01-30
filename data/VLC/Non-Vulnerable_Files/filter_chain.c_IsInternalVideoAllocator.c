}
static bool IsInternalVideoAllocator( chained_filter_t *p_filter )
{
    return p_filter->filter.pf_video_buffer_new == VideoBufferNew;
}
