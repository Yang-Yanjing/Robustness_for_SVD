AVFILTER_DEFINE_CLASS(smptebars);
static void smptebars_fill_picture(AVFilterContext *ctx, AVFrame *picref)
{
    TestSourceContext *test = ctx->priv;
    int r_w, r_h, w_h, p_w, p_h, i, tmp, x = 0;
    const AVPixFmtDescriptor *pixdesc = av_pix_fmt_desc_get(picref->format);
    av_frame_set_colorspace(picref, AVCOL_SPC_BT470BG);
    r_w = FFALIGN((test->w + 6) / 7, 1 << pixdesc->log2_chroma_w);
    r_h = FFALIGN(test->h * 2 / 3, 1 << pixdesc->log2_chroma_h);
    w_h = FFALIGN(test->h * 3 / 4 - r_h,  1 << pixdesc->log2_chroma_h);
    p_w = FFALIGN(r_w * 5 / 4, 1 << pixdesc->log2_chroma_w);
    p_h = test->h - w_h - r_h;
    for (i = 0; i < 7; i++) {
        draw_bar(test, rainbow[i], x, 0,   r_w, r_h, picref);
        draw_bar(test, wobnair[i], x, r_h, r_w, w_h, picref);
        x += r_w;
    }
    x = 0;
    draw_bar(test, i_pixel, x, r_h + w_h, p_w, p_h, picref);
    x += p_w;
    draw_bar(test, white, x, r_h + w_h, p_w, p_h, picref);
    x += p_w;
    draw_bar(test, q_pixel, x, r_h + w_h, p_w, p_h, picref);
    x += p_w;
    tmp = FFALIGN(5 * r_w - x,  1 << pixdesc->log2_chroma_w);
    draw_bar(test, black0, x, r_h + w_h, tmp, p_h, picref);
    x += tmp;
    tmp = FFALIGN(r_w / 3,  1 << pixdesc->log2_chroma_w);
    draw_bar(test, neg4ire, x, r_h + w_h, tmp, p_h, picref);
    x += tmp;
    draw_bar(test, black0, x, r_h + w_h, tmp, p_h, picref);
    x += tmp;
    draw_bar(test, pos4ire, x, r_h + w_h, tmp, p_h, picref);
    x += tmp;
    draw_bar(test, black0, x, r_h + w_h, test->w - x, p_h, picref);
}
