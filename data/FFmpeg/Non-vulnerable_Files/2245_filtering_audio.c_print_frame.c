static void print_frame(const AVFrame *frame)
{
    const int n = frame->nb_samples * av_get_channel_layout_nb_channels(av_frame_get_channel_layout(frame));
    const uint16_t *p     = (uint16_t*)frame->data[0];
    const uint16_t *p_end = p + n;
    while (p < p_end) {
        fputc(*p    & 0xff, stdout);
        fputc(*p>>8 & 0xff, stdout);
        p++;
    }
    fflush(stdout);
}
