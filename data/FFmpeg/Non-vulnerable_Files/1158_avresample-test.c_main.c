int main(int argc, char **argv)
{
    AVAudioResampleContext *s;
    AVLFG rnd;
    int ret = 0;
    uint8_t *in_buf = NULL;
    uint8_t *out_buf = NULL;
    unsigned int in_buf_size;
    unsigned int out_buf_size;
    uint8_t  *in_data[AVRESAMPLE_MAX_CHANNELS] = { 0 };
    uint8_t *out_data[AVRESAMPLE_MAX_CHANNELS] = { 0 };
    int in_linesize;
    int out_linesize;
    uint64_t in_ch_layout;
    int in_channels;
    enum AVSampleFormat in_fmt;
    int in_rate;
    uint64_t out_ch_layout;
    int out_channels;
    enum AVSampleFormat out_fmt;
    int out_rate;
    int num_formats, num_rates, num_layouts;
    int i, j, k, l, m, n;
    num_formats = 2;
    num_rates   = 2;
    num_layouts = 2;
    if (argc > 1) {
        if (!av_strncasecmp(argv[1], "-h", 3)) {
            av_log(NULL, AV_LOG_INFO, "Usage: avresample-test [<num formats> "
                   "[<num sample rates> [<num channel layouts>]]]\n"
                   "Default is 2 2 2\n");
            return 0;
        }
        num_formats = strtol(argv[1], NULL, 0);
        num_formats = av_clip(num_formats, 1, FF_ARRAY_ELEMS(formats));
    }
    if (argc > 2) {
        num_rates = strtol(argv[2], NULL, 0);
        num_rates = av_clip(num_rates, 1, FF_ARRAY_ELEMS(rates));
    }
    if (argc > 3) {
        num_layouts = strtol(argv[3], NULL, 0);
        num_layouts = av_clip(num_layouts, 1, FF_ARRAY_ELEMS(layouts));
    }
    av_log_set_level(AV_LOG_DEBUG);
    av_lfg_init(&rnd, 0xC0FFEE);
    in_buf_size = av_samples_get_buffer_size(&in_linesize, 8, 48000 * 6,
                                             AV_SAMPLE_FMT_DBLP, 0);
    out_buf_size = in_buf_size;
    in_buf = av_malloc(in_buf_size);
    if (!in_buf)
        goto end;
    out_buf = av_malloc(out_buf_size);
    if (!out_buf)
        goto end;
    s = avresample_alloc_context();
    if (!s) {
        av_log(NULL, AV_LOG_ERROR, "Error allocating AVAudioResampleContext\n");
        ret = 1;
        goto end;
    }
    for (i = 0; i < num_formats; i++) {
        in_fmt = formats[i];
        for (k = 0; k < num_layouts; k++) {
            in_ch_layout = layouts[k];
            in_channels  = av_get_channel_layout_nb_channels(in_ch_layout);
            for (m = 0; m < num_rates; m++) {
                in_rate = rates[m];
                ret = av_samples_fill_arrays(in_data, &in_linesize, in_buf,
                                             in_channels, in_rate * 6,
                                             in_fmt, 0);
                if (ret < 0) {
                    av_log(s, AV_LOG_ERROR, "failed in_data fill arrays\n");
                    goto end;
                }
                audiogen(&rnd, (void **)in_data, in_fmt, in_channels, in_rate, in_rate * 6);
                for (j = 0; j < num_formats; j++) {
                    out_fmt = formats[j];
                    for (l = 0; l < num_layouts; l++) {
                        out_ch_layout = layouts[l];
                        out_channels  = av_get_channel_layout_nb_channels(out_ch_layout);
                        for (n = 0; n < num_rates; n++) {
                            out_rate = rates[n];
                            av_log(NULL, AV_LOG_INFO, "%s to %s, %d to %d channels, %d Hz to %d Hz\n",
                                   av_get_sample_fmt_name(in_fmt), av_get_sample_fmt_name(out_fmt),
                                   in_channels, out_channels, in_rate, out_rate);
                            ret = av_samples_fill_arrays(out_data, &out_linesize,
                                                         out_buf, out_channels,
                                                         out_rate * 6, out_fmt, 0);
                            if (ret < 0) {
                                av_log(s, AV_LOG_ERROR, "failed out_data fill arrays\n");
                                goto end;
                            }
                            av_opt_set_int(s, "in_channel_layout",  in_ch_layout,  0);
                            av_opt_set_int(s, "in_sample_fmt",      in_fmt,        0);
                            av_opt_set_int(s, "in_sample_rate",     in_rate,       0);
                            av_opt_set_int(s, "out_channel_layout", out_ch_layout, 0);
                            av_opt_set_int(s, "out_sample_fmt",     out_fmt,       0);
                            av_opt_set_int(s, "out_sample_rate",    out_rate,      0);
                            av_opt_set_int(s, "internal_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
                            ret = avresample_open(s);
                            if (ret < 0) {
                                av_log(s, AV_LOG_ERROR, "Error opening context\n");
                                goto end;
                            }
                            ret = avresample_convert(s, out_data, out_linesize, out_rate * 6,
                                                         in_data,  in_linesize,  in_rate * 6);
                            if (ret < 0) {
                                char errbuf[256];
                                av_strerror(ret, errbuf, sizeof(errbuf));
                                av_log(NULL, AV_LOG_ERROR, "%s\n", errbuf);
                                goto end;
                            }
                            av_log(NULL, AV_LOG_INFO, "Converted %d samples to %d samples\n",
                                   in_rate * 6, ret);
                            if (avresample_get_delay(s) > 0)
                                av_log(NULL, AV_LOG_INFO, "%d delay samples not converted\n",
                                       avresample_get_delay(s));
                            if (avresample_available(s) > 0)
                                av_log(NULL, AV_LOG_INFO, "%d samples available for output\n",
                                       avresample_available(s));
                            av_log(NULL, AV_LOG_INFO, "\n");
                            avresample_close(s);
                        }
                    }
                }
            }
        }
    }
    ret = 0;
end:
    av_freep(&in_buf);
    av_freep(&out_buf);
    avresample_free(&s);
    return ret;
}
