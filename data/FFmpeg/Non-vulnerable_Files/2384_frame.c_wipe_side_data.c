static void wipe_side_data(AVFrame *frame)
{
    int i;
    for (i = 0; i < frame->nb_side_data; i++) {
        free_side_data(&frame->side_data[i]);
    }
    frame->nb_side_data = 0;
    av_freep(&frame->side_data);
}
