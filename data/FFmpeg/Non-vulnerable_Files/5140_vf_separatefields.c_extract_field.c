static void extract_field(AVFrame *frame, int nb_planes, int type)
{
    int i;
    for (i = 0; i < nb_planes; i++) {
        if (type)
            frame->data[i] = frame->data[i] + frame->linesize[i];
        frame->linesize[i] *= 2;
    }
}
