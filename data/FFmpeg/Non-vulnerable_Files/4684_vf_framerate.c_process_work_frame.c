static int process_work_frame(AVFilterContext *ctx, int stop)
{
    FrameRateContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int64_t work_next_pts;
    AVFrame *copy_src1, *copy_src2, *work;
    int interpolate;
    ff_dlog(ctx, "process_work_frame()\n");
    ff_dlog(ctx, "process_work_frame() pending_input_frames %d\n", s->pending_srce_frames);
    if (s->srce[s->prev]) ff_dlog(ctx, "process_work_frame() srce prev pts:%"PRId64"\n", s->srce[s->prev]->pts);
    if (s->srce[s->crnt]) ff_dlog(ctx, "process_work_frame() srce crnt pts:%"PRId64"\n", s->srce[s->crnt]->pts);
    if (s->srce[s->next]) ff_dlog(ctx, "process_work_frame() srce next pts:%"PRId64"\n", s->srce[s->next]->pts);
    if (!s->srce[s->crnt]) {
        
        ff_dlog(ctx, "process_work_frame() no current frame cached: move on to next frame, do not output a frame\n");
        next_source(ctx);
        return 0;
    }
    work_next_pts = s->pts + s->average_dest_pts_delta;
    ff_dlog(ctx, "process_work_frame() work crnt pts:%"PRId64"\n", s->pts);
    ff_dlog(ctx, "process_work_frame() work next pts:%"PRId64"\n", work_next_pts);
    if (s->srce[s->prev])
        ff_dlog(ctx, "process_work_frame() srce prev pts:%"PRId64" at dest time base:%u/%u\n",
            s->srce_pts_dest[s->prev], s->dest_time_base.num, s->dest_time_base.den);
    if (s->srce[s->crnt])
        ff_dlog(ctx, "process_work_frame() srce crnt pts:%"PRId64" at dest time base:%u/%u\n",
            s->srce_pts_dest[s->crnt], s->dest_time_base.num, s->dest_time_base.den);
    if (s->srce[s->next])
        ff_dlog(ctx, "process_work_frame() srce next pts:%"PRId64" at dest time base:%u/%u\n",
            s->srce_pts_dest[s->next], s->dest_time_base.num, s->dest_time_base.den);
    av_assert0(s->srce[s->next]);
    
    if (!s->flush && s->pts >= s->srce_pts_dest[s->next]) {
        ff_dlog(ctx, "process_work_frame() work crnt pts >= srce next pts: SKIP FRAME, move on to next frame, do not output a frame\n");
        next_source(ctx);
        s->pending_srce_frames--;
        return 0;
    }
    
    interpolate = (int) ((s->pts - s->srce_pts_dest[s->crnt]) * 256.0 / s->average_srce_pts_dest_delta);
    ff_dlog(ctx, "process_work_frame() interpolate:%d/256\n", interpolate);
    copy_src1 = s->srce[s->crnt];
    if (interpolate > s->interp_end) {
        ff_dlog(ctx, "process_work_frame() source is:NEXT\n");
        copy_src1 = s->srce[s->next];
    }
    if (s->srce[s->prev] && interpolate < -s->interp_end) {
        ff_dlog(ctx, "process_work_frame() source is:PREV\n");
        copy_src1 = s->srce[s->prev];
    }
    
    if ((interpolate >= s->interp_start && interpolate <= s->interp_end) || (interpolate <= -s->interp_start && interpolate >= -s->interp_end)) {
        double interpolate_scene_score = 0;
        if (interpolate > 0) {
            ff_dlog(ctx, "process_work_frame() interpolate source is:NEXT\n");
            copy_src2 = s->srce[s->next];
        } else {
            ff_dlog(ctx, "process_work_frame() interpolate source is:PREV\n");
            copy_src2 = s->srce[s->prev];
        }
        if ((s->flags & FRAMERATE_FLAG_SCD) && copy_src2) {
            interpolate_scene_score = get_scene_score(ctx, copy_src1, copy_src2);
            ff_dlog(ctx, "process_work_frame() interpolate scene score:%f\n", interpolate_scene_score);
        }
        
        if (interpolate_scene_score < s->scene_score && copy_src2) {
            uint16_t src2_factor = abs(interpolate);
            uint16_t src1_factor = 256 - src2_factor;
            int plane, line, pixel;
            
            work = ff_get_video_buffer(outlink, outlink->w, outlink->h);
            if (!work)
                return AVERROR(ENOMEM);
            av_frame_copy_props(work, s->srce[s->crnt]);
            ff_dlog(ctx, "process_work_frame() INTERPOLATE to create work frame\n");
            for (plane = 0; plane < 4 && copy_src1->data[plane] && copy_src2->data[plane]; plane++) {
                int cpy_line_width = s->line_size[plane];
                uint8_t *cpy_src1_data = copy_src1->data[plane];
                int cpy_src1_line_size = copy_src1->linesize[plane];
                uint8_t *cpy_src2_data = copy_src2->data[plane];
                int cpy_src2_line_size = copy_src2->linesize[plane];
                int cpy_src_h = (plane > 0 && plane < 3) ? (copy_src1->height >> s->vsub) : (copy_src1->height);
                uint8_t *cpy_dst_data = work->data[plane];
                int cpy_dst_line_size = work->linesize[plane];
                if (plane <1 || plane >2) {
                    
                    for (line = 0; line < cpy_src_h; line++) {
                        for (pixel = 0; pixel < cpy_line_width; pixel++) {
                            
                            
                            
                            cpy_dst_data[pixel] = ((cpy_src1_data[pixel] * src1_factor) + (cpy_src2_data[pixel] * src2_factor) + 128) >> 8;
                        }
                        cpy_src1_data += cpy_src1_line_size;
                        cpy_src2_data += cpy_src2_line_size;
                        cpy_dst_data += cpy_dst_line_size;
                    }
                } else {
                    
                    for (line = 0; line < cpy_src_h; line++) {
                        for (pixel = 0; pixel < cpy_line_width; pixel++) {
                            
                            
                            
                            cpy_dst_data[pixel] = (((cpy_src1_data[pixel] - 128) * src1_factor) + ((cpy_src2_data[pixel] - 128) * src2_factor) + 32896) >> 8;
                        }
                        cpy_src1_data += cpy_src1_line_size;
                        cpy_src2_data += cpy_src2_line_size;
                        cpy_dst_data += cpy_dst_line_size;
                    }
                }
            }
            goto copy_done;
        }
        else {
            ff_dlog(ctx, "process_work_frame() CUT - DON'T INTERPOLATE\n");
        }
    }
    ff_dlog(ctx, "process_work_frame() COPY to the work frame\n");
    
    work = av_frame_clone(copy_src1);
    if (!work)
        return AVERROR(ENOMEM);
copy_done:
    work->pts = s->pts;
    
    if (!s->flush && (work_next_pts + s->average_dest_pts_delta) < (s->srce_pts_dest[s->crnt] + s->average_srce_pts_dest_delta)) {
        ff_dlog(ctx, "process_work_frame() REPEAT FRAME\n");
    } else {
        ff_dlog(ctx, "process_work_frame() CONSUME FRAME, move to next frame\n");
        s->pending_srce_frames--;
        next_source(ctx);
    }
    ff_dlog(ctx, "process_work_frame() output a frame\n");
    s->dest_frame_num++;
    if (stop)
        s->pending_end_frame = 0;
    s->last_dest_frame_pts = work->pts;
    return ff_filter_frame(ctx->outputs[0], work);
}
