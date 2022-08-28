static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    FadeContext *s       = ctx->priv;
    double frame_timestamp = frame->pts == AV_NOPTS_VALUE ? -1 : frame->pts * av_q2d(inlink->time_base);
    
    if (s->fade_state == VF_FADE_WAITING) {
        s->factor=0;
        if (frame_timestamp >= s->start_time/(double)AV_TIME_BASE
            && inlink->frame_count >= s->start_frame) {
            
            s->fade_state = VF_FADE_FADING;
            
            if (s->start_time == 0 && s->start_frame != 0) {
                s->start_time = frame_timestamp*(double)AV_TIME_BASE;
            }
            
            if (s->start_time != 0 && s->start_frame == 0) {
                s->start_frame = inlink->frame_count;
            }
        }
    }
    if (s->fade_state == VF_FADE_FADING) {
        if (s->duration == 0) {
            
            s->factor = (inlink->frame_count - s->start_frame) * s->fade_per_frame;
            if (inlink->frame_count > s->start_frame + s->nb_frames) {
                s->fade_state = VF_FADE_DONE;
            }
        } else {
            
            s->factor = (frame_timestamp - s->start_time/(double)AV_TIME_BASE)
                            * (float) UINT16_MAX / (s->duration/(double)AV_TIME_BASE);
            if (frame_timestamp > s->start_time/(double)AV_TIME_BASE
                                  + s->duration/(double)AV_TIME_BASE) {
                s->fade_state = VF_FADE_DONE;
            }
        }
    }
    if (s->fade_state == VF_FADE_DONE) {
        s->factor=UINT16_MAX;
    }
    s->factor = av_clip_uint16(s->factor);
    
    if (s->type == FADE_OUT) {
        s->factor=UINT16_MAX-s->factor;
    }
    if (s->factor < UINT16_MAX) {
        if (s->alpha) {
            ctx->internal->execute(ctx, filter_slice_alpha, frame, NULL,
                                FFMIN(frame->height, ctx->graph->nb_threads));
        } else if (s->is_packed_rgb && !s->black_fade) {
            ctx->internal->execute(ctx, filter_slice_rgb, frame, NULL,
                                   FFMIN(frame->height, ctx->graph->nb_threads));
        } else {
            
            ctx->internal->execute(ctx, filter_slice_luma, frame, NULL,
                                FFMIN(frame->height, ctx->graph->nb_threads));
            if (frame->data[1] && frame->data[2]) {
                
                ctx->internal->execute(ctx, filter_slice_chroma, frame, NULL,
                                    FFMIN(frame->height, ctx->graph->nb_threads));
            }
        }
    }
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
