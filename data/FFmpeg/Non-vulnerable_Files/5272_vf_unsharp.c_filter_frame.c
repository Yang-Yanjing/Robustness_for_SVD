static int filter_frame(AVFilterLink *link, AVFrame *in)
{
    UnsharpContext *unsharp = link->dst->priv;
    AVFilterLink *outlink   = link->dst->outputs[0];
    AVFrame *out;
    int ret = 0;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    if (CONFIG_OPENCL && unsharp->opencl) {
        ret = ff_opencl_unsharp_process_inout_buf(link->dst, in, out);
        if (ret < 0)
            goto end;
    }
    ret = unsharp->apply_unsharp(link->dst, in, out);
end:
    av_frame_free(&in);
    if (ret < 0)
        return ret;
    return ff_filter_frame(outlink, out);
}
