static int filter_frame(AVFilterLink *link, AVFrame *in)
{
    ScaleContext *scale = link->dst->priv;
    AVFilterLink *outlink = link->dst->outputs[0];
    AVFrame *out;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);
    char buf[32];
    int in_range;
    if (av_frame_get_colorspace(in) == AVCOL_SPC_YCGCO)
        av_log(link->dst, AV_LOG_WARNING, "Detected unsupported YCgCo colorspace.\n");
    if(   in->width  != link->w
       || in->height != link->h
       || in->format != link->format) {
        int ret;
        snprintf(buf, sizeof(buf)-1, "%d", outlink->w);
        av_opt_set(scale, "w", buf, 0);
        snprintf(buf, sizeof(buf)-1, "%d", outlink->h);
        av_opt_set(scale, "h", buf, 0);
        link->dst->inputs[0]->format = in->format;
        link->dst->inputs[0]->w      = in->width;
        link->dst->inputs[0]->h      = in->height;
        if ((ret = config_props(outlink)) < 0)
            return ret;
    }
    if (!scale->sws)
        return ff_filter_frame(outlink, in);
    scale->hsub = desc->log2_chroma_w;
    scale->vsub = desc->log2_chroma_h;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    out->width  = outlink->w;
    out->height = outlink->h;
    if(scale->output_is_pal)
        avpriv_set_systematic_pal2((uint32_t*)out->data[1], outlink->format == AV_PIX_FMT_PAL8 ? AV_PIX_FMT_BGR8 : outlink->format);
    in_range = av_frame_get_color_range(in);
    if (   scale->in_color_matrix
        || scale->out_color_matrix
        || scale-> in_range != AVCOL_RANGE_UNSPECIFIED
        || in_range != AVCOL_RANGE_UNSPECIFIED
        || scale->out_range != AVCOL_RANGE_UNSPECIFIED) {
        int in_full, out_full, brightness, contrast, saturation;
        const int *inv_table, *table;
        sws_getColorspaceDetails(scale->sws, (int **)&inv_table, &in_full,
                                 (int **)&table, &out_full,
                                 &brightness, &contrast, &saturation);
        if (scale->in_color_matrix)
            inv_table = parse_yuv_type(scale->in_color_matrix, av_frame_get_colorspace(in));
        if (scale->out_color_matrix)
            table     = parse_yuv_type(scale->out_color_matrix, AVCOL_SPC_UNSPECIFIED);
        else if (scale->in_color_matrix)
            table = inv_table;
        if (scale-> in_range != AVCOL_RANGE_UNSPECIFIED)
            in_full  = (scale-> in_range == AVCOL_RANGE_JPEG);
        else if (in_range != AVCOL_RANGE_UNSPECIFIED)
            in_full  = (in_range == AVCOL_RANGE_JPEG);
        if (scale->out_range != AVCOL_RANGE_UNSPECIFIED)
            out_full = (scale->out_range == AVCOL_RANGE_JPEG);
        sws_setColorspaceDetails(scale->sws, inv_table, in_full,
                                 table, out_full,
                                 brightness, contrast, saturation);
        if (scale->isws[0])
            sws_setColorspaceDetails(scale->isws[0], inv_table, in_full,
                                     table, out_full,
                                     brightness, contrast, saturation);
        if (scale->isws[1])
            sws_setColorspaceDetails(scale->isws[1], inv_table, in_full,
                                     table, out_full,
                                     brightness, contrast, saturation);
    }
    av_reduce(&out->sample_aspect_ratio.num, &out->sample_aspect_ratio.den,
              (int64_t)in->sample_aspect_ratio.num * outlink->h * link->w,
              (int64_t)in->sample_aspect_ratio.den * outlink->w * link->h,
              INT_MAX);
    if(scale->interlaced>0 || (scale->interlaced<0 && in->interlaced_frame)){
        scale_slice(link, out, in, scale->isws[0], 0, (link->h+1)/2, 2, 0);
        scale_slice(link, out, in, scale->isws[1], 0,  link->h   /2, 2, 1);
    }else{
        scale_slice(link, out, in, scale->sws, 0, link->h, 1, 0);
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
