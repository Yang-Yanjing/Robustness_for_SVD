static av_cold int init(AVFilterContext *ctx)
{
    AFormatContext *s = ctx->priv;
    PARSE_FORMATS(s->formats_str, enum AVSampleFormat, s->formats,
                  ff_add_format, av_get_sample_fmt, AV_SAMPLE_FMT_NONE, "sample format");
    PARSE_FORMATS(s->sample_rates_str, int, s->sample_rates, ff_add_format,
                  get_sample_rate, 0, "sample rate");
    PARSE_FORMATS(s->channel_layouts_str, uint64_t, s->channel_layouts,
                  ff_add_channel_layout, av_get_channel_layout, 0,
                  "channel layout");
    return 0;
}
