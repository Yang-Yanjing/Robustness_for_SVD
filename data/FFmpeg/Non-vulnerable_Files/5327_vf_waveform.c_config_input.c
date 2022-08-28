static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    WaveformContext *s = ctx->priv;
    s->desc  = av_pix_fmt_desc_get(inlink->format);
    s->ncomp = s->desc->nb_components;
    s->bits = s->desc->comp[0].depth_minus1 + 1;
    s->max = 1 << s->bits;
    s->intensity = s->fintensity * (s->max - 1);
    switch (s->filter) {
    case LOWPASS:
            s->size = 256;
            s->waveform = s->bits > 8 ? lowpass16 : lowpass; break;
    case FLAT:
            s->size = 256 * 3;
            s->waveform = flat;    break;
    case AFLAT:
            s->size = 256 * 2;
            s->waveform = aflat;   break;
    case CHROMA:
            s->size = 256 * 2;
            s->waveform = chroma;  break;
    case ACHROMA:
            s->size = 256;
            s->waveform = achroma; break;
    case COLOR:
            s->size = 256;
            s->waveform = s->bits > 8 ?   color16 :   color; break;
    }
    s->size = s->size << (s->bits - 8);
    switch (inlink->format) {
    case AV_PIX_FMT_GBRAP:
    case AV_PIX_FMT_GBRP:
    case AV_PIX_FMT_GBRP9:
    case AV_PIX_FMT_GBRP10:
        s->bg_color = black_gbrp_color;
        break;
    default:
        s->bg_color = black_yuva_color;
    }
    return 0;
}
