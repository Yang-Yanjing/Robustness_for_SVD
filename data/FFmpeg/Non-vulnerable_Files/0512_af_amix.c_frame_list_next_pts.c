static int64_t frame_list_next_pts(FrameList *frame_list)
{
    if (!frame_list->list)
        return AV_NOPTS_VALUE;
    return frame_list->list->pts;
}
