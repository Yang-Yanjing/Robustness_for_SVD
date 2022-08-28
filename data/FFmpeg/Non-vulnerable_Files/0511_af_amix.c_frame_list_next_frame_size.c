static int frame_list_next_frame_size(FrameList *frame_list)
{
    if (!frame_list->list)
        return 0;
    return frame_list->list->nb_samples;
}
