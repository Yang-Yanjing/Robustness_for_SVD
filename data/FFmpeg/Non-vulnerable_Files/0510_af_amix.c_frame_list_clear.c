static void frame_list_clear(FrameList *frame_list)
{
    if (frame_list) {
        while (frame_list->list) {
            FrameInfo *info = frame_list->list;
            frame_list->list = info->next;
            av_free(info);
        }
        frame_list->nb_frames  = 0;
        frame_list->nb_samples = 0;
        frame_list->end        = NULL;
    }
}
