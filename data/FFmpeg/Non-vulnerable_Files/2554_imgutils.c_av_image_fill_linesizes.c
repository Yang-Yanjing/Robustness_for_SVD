int av_image_fill_linesizes(int linesizes[4], enum AVPixelFormat pix_fmt, int width)
{
    int i, ret;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    int max_step     [4];       
    int max_step_comp[4];       
    memset(linesizes, 0, 4*sizeof(linesizes[0]));
    if (!desc || desc->flags & AV_PIX_FMT_FLAG_HWACCEL)
        return AVERROR(EINVAL);
    av_image_fill_max_pixsteps(max_step, max_step_comp, desc);
    for (i = 0; i < 4; i++) {
        if ((ret = image_get_linesize(width, i, max_step[i], max_step_comp[i], desc)) < 0)
            return ret;
        linesizes[i] = ret;
    }
    return 0;
}
