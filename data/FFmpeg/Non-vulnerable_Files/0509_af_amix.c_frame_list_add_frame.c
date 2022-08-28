static int frame_list_add_frame(FrameList *frame_list, int nb_samples, int64_t pts)
{
    FrameInfo *info = av_malloc(sizeof(*info));
    if (!info)
        return AVERROR(ENOMEM);
    info->nb_samples = nb_samples;
    info->pts        = pts;
    info->next       = NULL;
    if (!frame_list->list) {
        frame_list->list = info;
        frame_list->end  = info;
    } else {
        av_assert0(frame_list->end);
        frame_list->end->next = info;
        frame_list->end       = info;
    }
    frame_list->nb_frames++;
    frame_list->nb_samples += nb_samples;
    return 0;
}
