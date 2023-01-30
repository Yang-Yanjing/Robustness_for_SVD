static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    CodecViewContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrameSideData *sd = av_frame_get_side_data(frame, AV_FRAME_DATA_MOTION_VECTORS);
    if (sd) {
        int i;
        const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
        for (i = 0; i < sd->size / sizeof(*mvs); i++) {
            const AVMotionVector *mv = &mvs[i];
            const int direction = mv->source > 0;
            if ((direction == 0 && (s->mv & MV_P_FOR)  && frame->pict_type == AV_PICTURE_TYPE_P) ||
                (direction == 0 && (s->mv & MV_B_FOR)  && frame->pict_type == AV_PICTURE_TYPE_B) ||
                (direction == 1 && (s->mv & MV_B_BACK) && frame->pict_type == AV_PICTURE_TYPE_B))
                draw_arrow(frame->data[0], mv->dst_x, mv->dst_y, mv->src_x, mv->src_y,
                           frame->width, frame->height, frame->linesize[0],
                           100, 0, mv->source > 0);
        }
    }
    return ff_filter_frame(outlink, frame);
}
