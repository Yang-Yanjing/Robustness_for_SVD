AVFILTER_DEFINE_CLASS(smptehdbars);
static void smptehdbars_fill_picture(AVFilterContext *ctx, AVFrame *picref)
{
    TestSourceContext *test = ctx->priv;
    int d_w, r_w, r_h, l_w, i, tmp, x = 0, y = 0;
    const AVPixFmtDescriptor *pixdesc = av_pix_fmt_desc_get(picref->format);
    av_frame_set_colorspace(picref, AVCOL_SPC_BT709);
    d_w = FFALIGN(test->w / 8, 1 << pixdesc->log2_chroma_w);
    r_h = FFALIGN(test->h * 7 / 12, 1 << pixdesc->log2_chroma_h);
    draw_bar(test, gray40, x, 0, d_w, r_h, picref);
    x += d_w;
    r_w = FFALIGN((((test->w + 3) / 4) * 3) / 7, 1 << pixdesc->log2_chroma_w);
    for (i = 0; i < 7; i++) {
        draw_bar(test, rainbowhd[i], x, 0, r_w, r_h, picref);
        x += r_w;
    }
    draw_bar(test, gray40, x, 0, test->w - x, r_h, picref);
    y = r_h;
    r_h = FFALIGN(test->h / 12, 1 << pixdesc->log2_chroma_h);
    draw_bar(test, cyan, 0, y, d_w, r_h, picref);
    x = d_w;
    draw_bar(test, i_pixel, x, y, r_w, r_h, picref);
    x += r_w;
    tmp = r_w * 6;
    draw_bar(test, rainbowhd[0], x, y, tmp, r_h, picref);
    x += tmp;
    l_w = x;
    draw_bar(test, blue, x, y, test->w - x, r_h, picref);
    y += r_h;
    draw_bar(test, yellow, 0, y, d_w, r_h, picref);
    x = d_w;
    draw_bar(test, q_pixel, x, y, r_w, r_h, picref);
    x += r_w;
    for (i = 0; i < tmp; i += 1 << pixdesc->log2_chroma_w) {
        uint8_t yramp[4] = {0};
        yramp[0] = i * 255 / tmp;
        yramp[1] = 128;
        yramp[2] = 128;
        yramp[3] = 255;
        draw_bar(test, yramp, x, y, 1 << pixdesc->log2_chroma_w, r_h, picref);
        x += 1 << pixdesc->log2_chroma_w;
    }
    draw_bar(test, red, x, y, test->w - x, r_h, picref);
    y += r_h;
    draw_bar(test, gray15, 0, y, d_w, test->h - y, picref);
    x = d_w;
    tmp = FFALIGN(r_w * 3 / 2, 1 << pixdesc->log2_chroma_w);
    draw_bar(test, black0, x, y, tmp, test->h - y, picref);
    x += tmp;
    tmp = FFALIGN(r_w * 2, 1 << pixdesc->log2_chroma_w);
    draw_bar(test, white, x, y, tmp, test->h - y, picref);
    x += tmp;
    tmp = FFALIGN(r_w * 5 / 6, 1 << pixdesc->log2_chroma_w);
    draw_bar(test, black0, x, y, tmp, test->h - y, picref);
    x += tmp;
    tmp = FFALIGN(r_w / 3, 1 << pixdesc->log2_chroma_w);
    draw_bar(test,   neg2, x, y, tmp, test->h - y, picref);
    x += tmp;
    draw_bar(test, black0, x, y, tmp, test->h - y, picref);
    x += tmp;
    draw_bar(test, black2, x, y, tmp, test->h - y, picref);
    x += tmp;
    draw_bar(test, black0, x, y, tmp, test->h - y, picref);
    x += tmp;
    draw_bar(test, black4, x, y, tmp, test->h - y, picref);
    x += tmp;
    r_w = l_w - x;
    draw_bar(test, black0, x, y, r_w, test->h - y, picref);
    x += r_w;
    draw_bar(test, gray15, x, y, test->w - x, test->h - y, picref);
}
