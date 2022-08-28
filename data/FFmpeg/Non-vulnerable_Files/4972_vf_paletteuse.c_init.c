static av_cold int init(AVFilterContext *ctx)
{
    PaletteUseContext *s = ctx->priv;
    s->dinput.repeatlast = 1; 
    s->dinput.process    = load_apply_palette;
    s->set_frame = set_frame_lut[s->color_search_method][s->dither];
    if (s->dither == DITHERING_BAYER) {
        int i;
        const int delta = 1 << (5 - s->bayer_scale); 
        for (i = 0; i < FF_ARRAY_ELEMS(s->ordered_dither); i++)
            s->ordered_dither[i] = (dither_value(i) >> s->bayer_scale) - delta;
    }
    return 0;
}
