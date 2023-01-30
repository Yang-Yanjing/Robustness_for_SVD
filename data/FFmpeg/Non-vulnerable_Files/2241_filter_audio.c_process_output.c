static int process_output(struct AVMD5 *md5, AVFrame *frame)
{
    int planar     = av_sample_fmt_is_planar(frame->format);
    int channels   = av_get_channel_layout_nb_channels(frame->channel_layout);
    int planes     = planar ? channels : 1;
    int bps        = av_get_bytes_per_sample(frame->format);
    int plane_size = bps * frame->nb_samples * (planar ? 1 : channels);
    int i, j;
    for (i = 0; i < planes; i++) {
        uint8_t checksum[16];
        av_md5_init(md5);
        av_md5_sum(checksum, frame->extended_data[i], plane_size);
        fprintf(stdout, "plane %d: 0x", i);
        for (j = 0; j < sizeof(checksum); j++)
            fprintf(stdout, "%02X", checksum[j]);
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
    return 0;
}
