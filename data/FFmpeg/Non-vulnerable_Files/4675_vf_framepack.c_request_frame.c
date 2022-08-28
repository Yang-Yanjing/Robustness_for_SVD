static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FramepackContext *s = ctx->priv;
    AVStereo3D *stereo;
    int ret, i;
    
    for (i = 0; i < 2; i++) {
        if (!s->input_views[i]) {
            ret = ff_request_frame(ctx->inputs[i]);
            if (ret < 0)
                return ret;
        }
    }
    if (s->format == AV_STEREO3D_FRAMESEQUENCE) {
        if (s->double_pts == AV_NOPTS_VALUE)
            s->double_pts = s->input_views[LEFT]->pts;
        for (i = 0; i < 2; i++) {
            
            s->input_views[i]->pts = s->double_pts++;
            
            stereo = av_stereo3d_create_side_data(s->input_views[i]);
            if (!stereo)
                return AVERROR(ENOMEM);
            stereo->type = s->format;
            
            ret = ff_filter_frame(outlink, s->input_views[i]);
            s->input_views[i] = NULL;
            if (ret < 0)
                return ret;
        }
        return ret;
    } else {
        AVFrame *dst = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!dst)
            return AVERROR(ENOMEM);
        spatial_frame_pack(s, dst);
        
        ret = av_frame_copy_props(dst, s->input_views[LEFT]);
        if (ret < 0) {
            av_frame_free(&dst);
            return ret;
        }
        for (i = 0; i < 2; i++)
            av_frame_free(&s->input_views[i]);
        
        stereo = av_stereo3d_create_side_data(dst);
        if (!stereo) {
            av_frame_free(&dst);
            return AVERROR(ENOMEM);
        }
        stereo->type = s->format;
        return ff_filter_frame(outlink, dst);
    }
}
