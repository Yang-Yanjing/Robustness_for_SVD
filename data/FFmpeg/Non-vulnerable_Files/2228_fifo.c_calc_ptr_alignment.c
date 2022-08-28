static int calc_ptr_alignment(AVFrame *frame)
{
    int planes = av_sample_fmt_is_planar(frame->format) ?
                 av_get_channel_layout_nb_channels(frame->channel_layout) : 1;
    int min_align = 128;
    int p;
    for (p = 0; p < planes; p++) {
        int cur_align = 128;
        while ((intptr_t)frame->extended_data[p] % cur_align)
            cur_align >>= 1;
        if (cur_align < min_align)
            min_align = cur_align;
    }
    return min_align;
}
