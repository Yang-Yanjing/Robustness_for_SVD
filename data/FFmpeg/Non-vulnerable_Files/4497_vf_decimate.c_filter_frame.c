static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    int scpos = -1, duppos = -1;
    int drop = INT_MIN, i, lowest = 0, ret;
    AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    DecimateContext *dm   = ctx->priv;
    AVFrame *prv;
    
    if (FF_INLINK_IDX(inlink) == INPUT_MAIN) {
        dm->queue[dm->fid].frame = in;
        dm->got_frame[INPUT_MAIN] = 1;
    } else {
        dm->clean_src[dm->fid] = in;
        dm->got_frame[INPUT_CLEANSRC] = 1;
    }
    if (!dm->got_frame[INPUT_MAIN] || (dm->ppsrc && !dm->got_frame[INPUT_CLEANSRC]))
        return 0;
    dm->got_frame[INPUT_MAIN] = dm->got_frame[INPUT_CLEANSRC] = 0;
    if (in) {
        
        prv = dm->fid ? dm->queue[dm->fid - 1].frame : dm->last;
        if (!prv)
            prv = in;
        calc_diffs(dm, &dm->queue[dm->fid], prv, in);
        if (++dm->fid != dm->cycle)
            return 0;
        av_frame_free(&dm->last);
        dm->last = av_frame_clone(in);
        dm->fid = 0;
        
        lowest = 0;
        for (i = 0; i < dm->cycle; i++) {
            if (dm->queue[i].totdiff > dm->scthresh)
                scpos = i;
            if (dm->queue[i].maxbdiff < dm->queue[lowest].maxbdiff)
                lowest = i;
        }
        if (dm->queue[lowest].maxbdiff < dm->dupthresh)
            duppos = lowest;
        drop = scpos >= 0 && duppos < 0 ? scpos : lowest;
    }
    
    if (av_log_get_level() >= AV_LOG_DEBUG) {
        av_log(ctx, AV_LOG_DEBUG, "1/%d frame drop:\n", dm->cycle);
        for (i = 0; i < dm->cycle && dm->queue[i].frame; i++) {
            av_log(ctx, AV_LOG_DEBUG,"  #%d: totdiff=%08"PRIx64" maxbdiff=%08"PRIx64"%s%s%s%s\n",
                   i + 1, dm->queue[i].totdiff, dm->queue[i].maxbdiff,
                   i == scpos  ? " sc"     : "",
                   i == duppos ? " dup"    : "",
                   i == lowest ? " lowest" : "",
                   i == drop   ? " [DROP]" : "");
        }
    }
    
    ret = 0;
    for (i = 0; i < dm->cycle && dm->queue[i].frame; i++) {
        if (i == drop) {
            if (dm->ppsrc)
                av_frame_free(&dm->clean_src[i]);
            av_frame_free(&dm->queue[i].frame);
        } else {
            AVFrame *frame = dm->queue[i].frame;
            if (frame->pts != AV_NOPTS_VALUE && dm->start_pts == AV_NOPTS_VALUE)
                dm->start_pts = frame->pts;
            if (dm->ppsrc) {
                av_frame_free(&frame);
                frame = dm->clean_src[i];
            }
            frame->pts = outlink->frame_count * dm->ts_unit +
                         (dm->start_pts == AV_NOPTS_VALUE ? 0 : dm->start_pts);
            ret = ff_filter_frame(outlink, frame);
            if (ret < 0)
                break;
        }
    }
    return ret;
}
