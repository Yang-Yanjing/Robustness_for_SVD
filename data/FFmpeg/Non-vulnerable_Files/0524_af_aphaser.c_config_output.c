PHASER(s32, int32_t)
static int config_output(AVFilterLink *outlink)
{
    AudioPhaserContext *s = outlink->src->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    s->delay_buffer_length = s->delay * 0.001 * inlink->sample_rate + 0.5;
    if (s->delay_buffer_length <= 0) {
        av_log(outlink->src, AV_LOG_ERROR, "delay is too small\n");
        return AVERROR(EINVAL);
    }
    s->delay_buffer = av_calloc(s->delay_buffer_length, sizeof(*s->delay_buffer) * inlink->channels);
    s->modulation_buffer_length = inlink->sample_rate / s->speed + 0.5;
    s->modulation_buffer = av_malloc_array(s->modulation_buffer_length, sizeof(*s->modulation_buffer));
    if (!s->modulation_buffer || !s->delay_buffer)
        return AVERROR(ENOMEM);
    ff_generate_wave_table(s->type, AV_SAMPLE_FMT_S32,
                           s->modulation_buffer, s->modulation_buffer_length,
                           1., s->delay_buffer_length, M_PI / 2.0);
    s->delay_pos = s->modulation_pos = 0;
    switch (inlink->format) {
    case AV_SAMPLE_FMT_DBL:  s->phaser = phaser_dbl;  break;
    case AV_SAMPLE_FMT_DBLP: s->phaser = phaser_dblp; break;
    case AV_SAMPLE_FMT_FLT:  s->phaser = phaser_flt;  break;
    case AV_SAMPLE_FMT_FLTP: s->phaser = phaser_fltp; break;
    case AV_SAMPLE_FMT_S16:  s->phaser = phaser_s16;  break;
    case AV_SAMPLE_FMT_S16P: s->phaser = phaser_s16p; break;
    case AV_SAMPLE_FMT_S32:  s->phaser = phaser_s32;  break;
    case AV_SAMPLE_FMT_S32P: s->phaser = phaser_s32p; break;
    default: av_assert0(0);
    }
    return 0;
}
