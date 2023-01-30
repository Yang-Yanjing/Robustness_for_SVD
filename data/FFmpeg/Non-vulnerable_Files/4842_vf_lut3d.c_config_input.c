static int config_input(AVFilterLink *inlink)
{
    int is16bit = 0;
    LUT3DContext *lut3d = inlink->dst->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    switch (inlink->format) {
    case AV_PIX_FMT_RGB48:
    case AV_PIX_FMT_BGR48:
    case AV_PIX_FMT_RGBA64:
    case AV_PIX_FMT_BGRA64:
        is16bit = 1;
    }
    ff_fill_rgba_map(lut3d->rgba_map, inlink->format);
    lut3d->step = av_get_padded_bits_per_pixel(desc) >> (3 + is16bit);
#define SET_FUNC(name) do {                             \
    if (is16bit) lut3d->interp = interp_16_##name;      \
    else         lut3d->interp = interp_8_##name;       \
} while (0)
    switch (lut3d->interpolation) {
    case INTERPOLATE_NEAREST:     SET_FUNC(nearest);        break;
    case INTERPOLATE_TRILINEAR:   SET_FUNC(trilinear);      break;
    case INTERPOLATE_TETRAHEDRAL: SET_FUNC(tetrahedral);    break;
    default:
        av_assert0(0);
    }
    return 0;
}
