static int process_frame(FFFrameSync *fs)
{
    AVFilterContext *ctx = fs->parent;
    AVFilterLink *outlink = ctx->outputs[0];
    MergePlanesContext *s = fs->opaque;
    AVFrame *in[4] = { NULL };
    AVFrame *out;
    int i, ret;
    for (i = 0; i < s->nb_inputs; i++) {
        if ((ret = ff_framesync_get_frame(&s->fs, i, &in[i], 0)) < 0)
            return ret;
    }
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    out->pts = av_rescale_q(s->fs.pts, s->fs.time_base, outlink->time_base);
    for (i = 0; i < s->nb_planes; i++) {
        const int input = s->map[i][1];
        const int plane = s->map[i][0];
        av_image_copy_plane(out->data[i], out->linesize[i],
                            in[input]->data[plane], in[input]->linesize[plane],
                            s->planewidth[i], s->planeheight[i]);
    }
    return ff_filter_frame(outlink, out);
}
