void av_frame_remove_side_data(AVFrame *frame, enum AVFrameSideDataType type)
{
    int i;
    for (i = 0; i < frame->nb_side_data; i++) {
        AVFrameSideData *sd = frame->side_data[i];
        if (sd->type == type) {
            free_side_data(&frame->side_data[i]);
            frame->side_data[i] = frame->side_data[frame->nb_side_data - 1];
            frame->nb_side_data--;
        }
    }
}
