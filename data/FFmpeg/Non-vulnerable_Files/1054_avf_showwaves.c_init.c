static av_cold int init(AVFilterContext *ctx)
{
    ShowWavesContext *showwaves = ctx->priv;
    if (!strcmp(ctx->filter->name, "showwavespic")) {
        showwaves->single_pic = 1;
        showwaves->mode = MODE_CENTERED_LINE;
    }
    switch (showwaves->mode) {
    case MODE_POINT:         showwaves->draw_sample = draw_sample_point; break;
    case MODE_LINE:          showwaves->draw_sample = draw_sample_line;  break;
    case MODE_P2P:           showwaves->draw_sample = draw_sample_p2p;   break;
    case MODE_CENTERED_LINE: showwaves->draw_sample = draw_sample_cline; break;
    default:
        return AVERROR_BUG;
    }
    return 0;
}
