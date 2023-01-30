static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ColorChannelMixerContext *s = ctx->priv;
    int i, j, size, *buffer;
    ff_fill_rgba_map(s->rgba_map, outlink->format);
    switch (outlink->format) {
    case AV_PIX_FMT_RGB48:
    case AV_PIX_FMT_BGR48:
    case AV_PIX_FMT_RGBA64:
    case AV_PIX_FMT_BGRA64:
        size = 65536;
        break;
    default:
        size = 256;
    }
    s->buffer = buffer = av_malloc(16 * size * sizeof(*s->buffer));
    if (!s->buffer)
        return AVERROR(ENOMEM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++, buffer += size)
            s->lut[i][j] = buffer;
    for (i = 0; i < size; i++) {
        s->lut[R][R][i] = round(i * s->rr);
        s->lut[R][G][i] = round(i * s->rg);
        s->lut[R][B][i] = round(i * s->rb);
        s->lut[R][A][i] = round(i * s->ra);
        s->lut[G][R][i] = round(i * s->gr);
        s->lut[G][G][i] = round(i * s->gg);
        s->lut[G][B][i] = round(i * s->gb);
        s->lut[G][A][i] = round(i * s->ga);
        s->lut[B][R][i] = round(i * s->br);
        s->lut[B][G][i] = round(i * s->bg);
        s->lut[B][B][i] = round(i * s->bb);
        s->lut[B][A][i] = round(i * s->ba);
        s->lut[A][R][i] = round(i * s->ar);
        s->lut[A][G][i] = round(i * s->ag);
        s->lut[A][B][i] = round(i * s->ab);
        s->lut[A][A][i] = round(i * s->aa);
    }
    return 0;
}
