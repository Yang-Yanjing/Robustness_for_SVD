AVFrameSideData *av_frame_get_side_data(const AVFrame *frame,
                                        enum AVFrameSideDataType type)
{
    int i;
    for (i = 0; i < frame->nb_side_data; i++) {
        if (frame->side_data[i]->type == type)
            return frame->side_data[i];
    }
    return NULL;
}
