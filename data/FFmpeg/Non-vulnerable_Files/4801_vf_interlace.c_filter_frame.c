static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    InterlaceContext *s = ctx->priv;
    AVFrame *out;
    int tff, ret;
    av_frame_free(&s->cur);
    s->cur  = s->next;
    s->next = buf;
    
    if (!s->cur || !s->next)
        return 0;
    if (s->cur->interlaced_frame) {
        av_log(ctx, AV_LOG_WARNING,
               "video is already interlaced, adjusting framerate only\n");
        out = av_frame_clone(s->cur);
        if (!out)
            return AVERROR(ENOMEM);
        out->pts /= 2;  
        ret = ff_filter_frame(outlink, out);
        return ret;
    }
    tff = (s->scan == MODE_TFF);
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out, s->cur);
    out->interlaced_frame = 1;
    out->top_field_first  = tff;
    out->pts             /= 2;  
    
    copy_picture_field(s, s->cur, out, inlink, tff ? FIELD_UPPER : FIELD_LOWER, s->lowpass);
    av_frame_free(&s->cur);
    
    copy_picture_field(s, s->next, out, inlink, tff ? FIELD_LOWER : FIELD_UPPER, s->lowpass);
    av_frame_free(&s->next);
    ret = ff_filter_frame(outlink, out);
    return ret;
}
