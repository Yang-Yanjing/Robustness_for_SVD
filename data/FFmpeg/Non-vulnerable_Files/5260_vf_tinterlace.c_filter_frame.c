static int filter_frame(AVFilterLink *inlink, AVFrame *picref)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    TInterlaceContext *tinterlace = ctx->priv;
    AVFrame *cur, *next, *out;
    int field, tff, ret;
    av_frame_free(&tinterlace->cur);
    tinterlace->cur  = tinterlace->next;
    tinterlace->next = picref;
    cur = tinterlace->cur;
    next = tinterlace->next;
    
    if (!tinterlace->cur)
        return 0;
    switch (tinterlace->mode) {
    case MODE_MERGE: 

        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, cur);
        out->height = outlink->h;
        out->interlaced_frame = 1;
        out->top_field_first = 1;
        out->sample_aspect_ratio = av_mul_q(cur->sample_aspect_ratio, av_make_q(2, 1));
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, FIELD_UPPER, tinterlace->flags);
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)next->data, next->linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, FIELD_LOWER, tinterlace->flags);
        av_frame_free(&tinterlace->next);
        break;
    case MODE_DROP_ODD:  
    case MODE_DROP_EVEN: 
        out = av_frame_clone(tinterlace->mode == MODE_DROP_EVEN ? cur : next);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_free(&tinterlace->next);
        break;
    case MODE_PAD: 

        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, cur);
        out->height = outlink->h;
        out->sample_aspect_ratio = av_mul_q(cur->sample_aspect_ratio, av_make_q(2, 1));
        field = (1 + tinterlace->frame) & 1 ? FIELD_UPPER : FIELD_LOWER;
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, field, tinterlace->flags);
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)tinterlace->black_data, tinterlace->black_linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, !field, tinterlace->flags);
        break;
        

    case MODE_INTERLEAVE_TOP:    
    case MODE_INTERLEAVE_BOTTOM: 
        tff = tinterlace->mode == MODE_INTERLEAVE_TOP;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, cur);
        out->interlaced_frame = 1;
        out->top_field_first = tff;
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_UPPER : FIELD_LOWER, 1, tff ? FIELD_UPPER : FIELD_LOWER,
                           tinterlace->flags);
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)next->data, next->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_LOWER : FIELD_UPPER, 1, tff ? FIELD_LOWER : FIELD_UPPER,
                           tinterlace->flags);
        av_frame_free(&tinterlace->next);
        break;
    case MODE_INTERLACEX2: 
        
        out = av_frame_clone(cur);
        if (!out)
            return AVERROR(ENOMEM);
        out->interlaced_frame = 1;
        if (cur->pts != AV_NOPTS_VALUE)
            out->pts = cur->pts*2;
        out->pts = av_rescale_q(out->pts, tinterlace->preout_time_base, outlink->time_base);
        if ((ret = ff_filter_frame(outlink, out)) < 0)
            return ret;
        
        tff = next->top_field_first;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, next);
        out->interlaced_frame = 1;
        out->top_field_first = !tff;
        if (next->pts != AV_NOPTS_VALUE && cur->pts != AV_NOPTS_VALUE)
            out->pts = cur->pts + next->pts;
        else
            out->pts = AV_NOPTS_VALUE;
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_LOWER : FIELD_UPPER, 1, tff ? FIELD_LOWER : FIELD_UPPER,
                           tinterlace->flags);
        
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)next->data, next->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_UPPER : FIELD_LOWER, 1, tff ? FIELD_UPPER : FIELD_LOWER,
                           tinterlace->flags);
        break;
    default:
        av_assert0(0);
    }
    out->pts = av_rescale_q(out->pts, tinterlace->preout_time_base, outlink->time_base);
    ret = ff_filter_frame(outlink, out);
    tinterlace->frame++;
    return ret;
}
