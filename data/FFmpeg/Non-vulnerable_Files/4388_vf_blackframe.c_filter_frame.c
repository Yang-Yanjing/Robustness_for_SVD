    av_dict_set(metadata, key, buf, 0)
static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    BlackFrameContext *s = ctx->priv;
    int x, i;
    int pblack = 0;
    uint8_t *p = frame->data[0];
    AVDictionary **metadata;
    char buf[32];
    for (i = 0; i < frame->height; i++) {
        for (x = 0; x < inlink->w; x++)
            s->nblack += p[x] < s->bthresh;
        p += frame->linesize[0];
    }
    if (frame->key_frame)
        s->last_keyframe = s->frame;
    pblack = s->nblack * 100 / (inlink->w * inlink->h);
    if (pblack >= s->bamount) {
        metadata = avpriv_frame_get_metadatap(frame);
        av_log(ctx, AV_LOG_INFO, "frame:%u pblack:%u pts:%"PRId64" t:%f "
               "type:%c last_keyframe:%d\n",
               s->frame, pblack, frame->pts,
               frame->pts == AV_NOPTS_VALUE ? -1 : frame->pts * av_q2d(inlink->time_base),
               av_get_picture_type_char(frame->pict_type), s->last_keyframe);
        SET_META("lavfi.blackframe.pblack", "%u", pblack);
    }
    s->frame++;
    s->nblack = 0;
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
