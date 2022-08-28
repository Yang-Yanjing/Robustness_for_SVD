static int process_frame(FFFrameSync *fs)
{
    AVFilterContext *ctx = fs->parent;
    AVFilterLink *outlink = ctx->outputs[0];
    StackContext *s = fs->opaque;
    AVFrame **in = s->frames;
    AVFrame *out;
    int i, p, ret, offset[4] = { 0 };
    for (i = 0; i < s->nb_inputs; i++) {
        if ((ret = ff_framesync_get_frame(&s->fs, i, &in[i], 0)) < 0)
            return ret;
    }
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    out->pts = av_rescale_q(s->fs.pts, s->fs.time_base, outlink->time_base);
    for (i = 0; i < s->nb_inputs; i++) {
        AVFilterLink *inlink = ctx->inputs[i];
        int linesize[4];
        int height[4];
        if ((ret = av_image_fill_linesizes(linesize, inlink->format, inlink->w)) < 0) {
            av_frame_free(&out);
            return ret;
        }
        height[1] = height[2] = FF_CEIL_RSHIFT(inlink->h, s->desc->log2_chroma_h);
        height[0] = height[3] = inlink->h;
        for (p = 0; p < s->nb_planes; p++) {
            if (s->is_vertical) {
                av_image_copy_plane(out->data[p] + offset[p] * out->linesize[p],
                                    out->linesize[p],
                                    in[i]->data[p],
                                    in[i]->linesize[p],
                                    linesize[p], height[p]);
                offset[p] += height[p];
            } else {
                av_image_copy_plane(out->data[p] + offset[p],
                                    out->linesize[p],
                                    in[i]->data[p],
                                    in[i]->linesize[p],
                                    linesize[p], height[p]);
                offset[p] += linesize[p];
            }
        }
    }
    return ff_filter_frame(outlink, out);
}
