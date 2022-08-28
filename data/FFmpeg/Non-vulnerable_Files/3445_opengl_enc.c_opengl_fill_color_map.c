static av_cold void opengl_fill_color_map(OpenGLContext *opengl)
{
    const AVPixFmtDescriptor *desc;
    int shift;
    enum AVPixelFormat pix_fmt = opengl->pix_fmt;
    
    if (pix_fmt == AV_PIX_FMT_RGB565 || pix_fmt == AV_PIX_FMT_BGR555 ||
        pix_fmt == AV_PIX_FMT_BGR8   || pix_fmt == AV_PIX_FMT_RGB8)
        pix_fmt = AV_PIX_FMT_RGB24;
    else if (pix_fmt == AV_PIX_FMT_BGR565 || pix_fmt == AV_PIX_FMT_RGB555)
        pix_fmt = AV_PIX_FMT_BGR24;
    desc = av_pix_fmt_desc_get(pix_fmt);
    if (!(desc->flags & AV_PIX_FMT_FLAG_RGB))
        return;
#define FILL_COMPONENT(i) { \
        shift = desc->comp[i].depth_minus1 >> 3; \
        opengl->color_map[(i << 2) + ((desc->comp[i].offset_plus1 - 1) >> shift)] = 1.0; \
    }
    memset(opengl->color_map, 0, sizeof(opengl->color_map));
    FILL_COMPONENT(0);
    FILL_COMPONENT(1);
    FILL_COMPONENT(2);
    if (desc->flags & AV_PIX_FMT_FLAG_ALPHA)
        FILL_COMPONENT(3);
#undef FILL_COMPONENT
}
