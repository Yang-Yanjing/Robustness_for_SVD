AVFILTER_DEFINE_CLASS(haldclutsrc);
static void haldclutsrc_fill_picture(AVFilterContext *ctx, AVFrame *frame)
{
    int i, j, k, x = 0, y = 0, is16bit = 0, step;
    uint32_t alpha = 0;
    const TestSourceContext *hc = ctx->priv;
    int level = hc->level;
    float scale;
    const int w = frame->width;
    const int h = frame->height;
    const uint8_t *data = frame->data[0];
    const int linesize  = frame->linesize[0];
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(frame->format);
    uint8_t rgba_map[4];
    av_assert0(w == h && w == level*level*level);
    ff_fill_rgba_map(rgba_map, frame->format);
    switch (frame->format) {
    case AV_PIX_FMT_RGB48:
    case AV_PIX_FMT_BGR48:
    case AV_PIX_FMT_RGBA64:
    case AV_PIX_FMT_BGRA64:
        is16bit = 1;
        alpha = 0xffff;
        break;
    case AV_PIX_FMT_RGBA:
    case AV_PIX_FMT_BGRA:
    case AV_PIX_FMT_ARGB:
    case AV_PIX_FMT_ABGR:
        alpha = 0xff;
        break;
    }
    step  = av_get_padded_bits_per_pixel(desc) >> (3 + is16bit);
    scale = ((float)(1 << (8*(is16bit+1))) - 1) / (level*level - 1);
#define LOAD_CLUT(nbits) do {                                                   \
    uint##nbits##_t *dst = ((uint##nbits##_t *)(data + y*linesize)) + x*step;   \
    dst[rgba_map[0]] = av_clip_uint##nbits(i * scale);                          \
    dst[rgba_map[1]] = av_clip_uint##nbits(j * scale);                          \
    dst[rgba_map[2]] = av_clip_uint##nbits(k * scale);                          \
    if (step == 4)                                                              \
        dst[rgba_map[3]] = alpha;                                               \
} while (0)
    level *= level;
    for (k = 0; k < level; k++) {
        for (j = 0; j < level; j++) {
            for (i = 0; i < level; i++) {
                if (!is16bit)
                    LOAD_CLUT(8);
                else
                    LOAD_CLUT(16);
                if (++x == w) {
                    x = 0;
                    y++;
                }
            }
        }
    }
}
