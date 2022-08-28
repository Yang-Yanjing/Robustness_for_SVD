void av_get_channel_layout_string(char *buf, int buf_size,
                                  int nb_channels, uint64_t channel_layout)
{
    AVBPrint bp;
    av_bprint_init_for_buffer(&bp, buf, buf_size);
    av_bprint_channel_layout(&bp, nb_channels, channel_layout);
}
