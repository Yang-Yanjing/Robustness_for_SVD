static int ff_filter_frame_framed(AVFilterLink *link, AVFrame *frame);
void ff_tlog_ref(void *ctx, AVFrame *ref, int end)
{
    av_unused char buf[16];
    ff_tlog(ctx,
            "ref[%p buf:%p data:%p linesize[%d, %d, %d, %d] pts:%"PRId64" pos:%"PRId64,
            ref, ref->buf, ref->data[0],
            ref->linesize[0], ref->linesize[1], ref->linesize[2], ref->linesize[3],
            ref->pts, av_frame_get_pkt_pos(ref));
    if (ref->width) {
        ff_tlog(ctx, " a:%d/%d s:%dx%d i:%c iskey:%d type:%c",
                ref->sample_aspect_ratio.num, ref->sample_aspect_ratio.den,
                ref->width, ref->height,
                !ref->interlaced_frame     ? 'P' :         
                ref->top_field_first ? 'T' : 'B',    
                ref->key_frame,
                av_get_picture_type_char(ref->pict_type));
    }
    if (ref->nb_samples) {
        ff_tlog(ctx, " cl:%"PRId64"d n:%d r:%d",
                ref->channel_layout,
                ref->nb_samples,
                ref->sample_rate);
    }
    ff_tlog(ctx, "]%s", end ? "\n" : "");
}
