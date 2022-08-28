    av_dict_set_int(metadata, key, value, 0);
static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    BBoxContext *bbox = ctx->priv;
    FFBoundingBox box;
    int has_bbox, w, h;
    has_bbox =
        ff_calculate_bounding_box(&box,
                                  frame->data[0], frame->linesize[0],
                                  inlink->w, inlink->h, bbox->min_val);
    w = box.x2 - box.x1 + 1;
    h = box.y2 - box.y1 + 1;
    av_log(ctx, AV_LOG_INFO,
           "n:%"PRId64" pts:%s pts_time:%s", inlink->frame_count,
           av_ts2str(frame->pts), av_ts2timestr(frame->pts, &inlink->time_base));
    if (has_bbox) {
        AVDictionary **metadata = avpriv_frame_get_metadatap(frame);
        SET_META("lavfi.bbox.x1", box.x1)
        SET_META("lavfi.bbox.x2", box.x2)
        SET_META("lavfi.bbox.y1", box.y1)
        SET_META("lavfi.bbox.y2", box.y2)
        SET_META("lavfi.bbox.w",  w)
        SET_META("lavfi.bbox.h",  h)
        av_log(ctx, AV_LOG_INFO,
               " x1:%d x2:%d y1:%d y2:%d w:%d h:%d"
               " crop=%d:%d:%d:%d drawbox=%d:%d:%d:%d",
               box.x1, box.x2, box.y1, box.y2, w, h,
               w, h, box.x1, box.y1,    
               box.x1, box.y1, w, h);   
    }
    av_log(ctx, AV_LOG_INFO, "\n");
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
