static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    EdgeDetectContext *edgedetect = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int p, direct = 0;
    AVFrame *out;
    if (edgedetect->mode != MODE_COLORMIX && av_frame_is_writable(in)) {
        direct = 1;
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    for (p = 0; p < edgedetect->nb_planes; p++) {
        struct plane_info *plane = &edgedetect->planes[p];
        uint8_t  *tmpbuf     = plane->tmpbuf;
        uint16_t *gradients  = plane->gradients;
        int8_t   *directions = plane->directions;
        
        gaussian_blur(ctx, inlink->w, inlink->h,
                      tmpbuf,      inlink->w,
                      in->data[p], in->linesize[p]);
        
        sobel(inlink->w, inlink->h,
              gradients, inlink->w,
              directions,inlink->w,
              tmpbuf,    inlink->w);
        

        memset(tmpbuf, 0, inlink->w * inlink->h);
        non_maximum_suppression(inlink->w, inlink->h,
                                tmpbuf,    inlink->w,
                                directions,inlink->w,
                                gradients, inlink->w);
        
        double_threshold(edgedetect->low_u8, edgedetect->high_u8,
                         inlink->w, inlink->h,
                         out->data[p], out->linesize[p],
                         tmpbuf,       inlink->w);
        if (edgedetect->mode == MODE_COLORMIX) {
            color_mix(inlink->w, inlink->h,
                      out->data[p], out->linesize[p],
                      in->data[p], in->linesize[p]);
        }
    }
    if (!direct)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
