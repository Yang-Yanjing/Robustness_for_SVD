}
static void FilterAllocationClean(filter_t *filter)
{
    filter->pf_video_buffer_new = NULL;
    filter->pf_video_buffer_del = NULL;
    filter->p_owner             = NULL;
}
