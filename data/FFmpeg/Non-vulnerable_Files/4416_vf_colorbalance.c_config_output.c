static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    ColorBalanceContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    double *shadows, *midtones, *highlights, *buffer;
    int i, r, g, b;
    buffer = av_malloc(256 * 3 * sizeof(*buffer));
    if (!buffer)
        return AVERROR(ENOMEM);
    shadows    = buffer + 256 * 0;
    midtones   = buffer + 256 * 1;
    highlights = buffer + 256 * 2;
    for (i = 0; i < 256; i++) {
        double low = av_clipd((i - 85.0) / -64.0 + 0.5, 0, 1) * 178.5;
        double mid = av_clipd((i - 85.0) /  64.0 + 0.5, 0, 1) *
                     av_clipd((i + 85.0 - 255.0) / -64.0 + 0.5, 0, 1) * 178.5;
        shadows[i] = low;
        midtones[i] = mid;
        highlights[255 - i] = low;
    }
    for (i = 0; i < 256; i++) {
        r = g = b = i;
        r = av_clip_uint8(r + s->cyan_red.shadows         * shadows[r]);
        r = av_clip_uint8(r + s->cyan_red.midtones        * midtones[r]);
        r = av_clip_uint8(r + s->cyan_red.highlights      * highlights[r]);
        g = av_clip_uint8(g + s->magenta_green.shadows    * shadows[g]);
        g = av_clip_uint8(g + s->magenta_green.midtones   * midtones[g]);
        g = av_clip_uint8(g + s->magenta_green.highlights * highlights[g]);
        b = av_clip_uint8(b + s->yellow_blue.shadows      * shadows[b]);
        b = av_clip_uint8(b + s->yellow_blue.midtones     * midtones[b]);
        b = av_clip_uint8(b + s->yellow_blue.highlights   * highlights[b]);
        s->lut[R][i] = r;
        s->lut[G][i] = g;
        s->lut[B][i] = b;
    }
    av_free(buffer);
    ff_fill_rgba_map(s->rgba_map, outlink->format);
    s->step = av_get_padded_bits_per_pixel(desc) >> 3;
    return 0;
}
