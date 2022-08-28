static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    DetelecineContext *s = ctx->priv;
    int i, len = 0, ret = 0, out = 0;
    if (s->start_time == AV_NOPTS_VALUE)
        s->start_time = inpicref->pts;
    if (s->nskip_fields >= 2) {
        s->nskip_fields -= 2;
        return 0;
    } else if (s->nskip_fields >= 1) {
        if (s->occupied) {
            s->occupied = 0;
            s->nskip_fields--;
        }
        else {
            for (i = 0; i < s->nb_planes; i++) {
                av_image_copy_plane(s->temp->data[i], s->temp->linesize[i],
                                    inpicref->data[i], inpicref->linesize[i],
                                    s->stride[i],
                                    s->planeheight[i]);
            }
            s->occupied = 1;
            s->nskip_fields--;
            return 0;
        }
    }
    if (s->nskip_fields == 0) {
        while(!len && s->pattern[s->pattern_pos]) {
            len = s->pattern[s->pattern_pos] - '0';
            s->pattern_pos++;
        }
        if (!s->pattern[s->pattern_pos])
            s->pattern_pos = 0;
        if(!len) { 
            av_frame_free(&inpicref);
            return 0;
        }
        if (s->occupied) {
            for (i = 0; i < s->nb_planes; i++) {
                
                av_image_copy_plane(s->frame->data[i] + s->frame->linesize[i] * s->first_field,
                                    s->frame->linesize[i] * 2,
                                    inpicref->data[i] + inpicref->linesize[i] * s->first_field,
                                    inpicref->linesize[i] * 2,
                                    s->stride[i],
                                    (s->planeheight[i] - s->first_field + 1) / 2);
                
                av_image_copy_plane(s->frame->data[i] + s->frame->linesize[i] * !s->first_field,
                                    s->frame->linesize[i] * 2,
                                    s->temp->data[i] + s->temp->linesize[i] * !s->first_field,
                                    s->temp->linesize[i] * 2,
                                    s->stride[i],
                                    (s->planeheight[i] - !s->first_field + 1) / 2);
            }
            len -= 2;
            for (i = 0; i < s->nb_planes; i++) {
                av_image_copy_plane(s->temp->data[i], s->temp->linesize[i],
                                    inpicref->data[i], inpicref->linesize[i],
                                    s->stride[i],
                                    s->planeheight[i]);
            }
            s->occupied = 1;
            out = 1;
        } else {
            if (len >= 2) {
                
                for (i = 0; i < s->nb_planes; i++)
                    av_image_copy_plane(s->frame->data[i], s->frame->linesize[i],
                                        inpicref->data[i], inpicref->linesize[i],
                                        s->stride[i],
                                        s->planeheight[i]);
                len -= 2;
                out = 1;
            } else if (len == 1) {
                
                for (i = 0; i < s->nb_planes; i++) {
                    av_image_copy_plane(s->frame->data[i] +
                                        s->frame->linesize[i] * s->first_field,
                                        s->frame->linesize[i] * 2,
                                        inpicref->data[i] +
                                        inpicref->linesize[i] * s->first_field,
                                        inpicref->linesize[i] * 2, s->stride[i],
                                        (s->planeheight[i] - s->first_field + 1) / 2);
                 }
                
                len--;
                out = 1;
            }
        }
        if (len == 1 && s->occupied)
        {
            len--;
            s->occupied = 0;
        }
    }
    s->nskip_fields = len;
    if (out) {
        AVFrame *frame = av_frame_clone(s->frame);
        if (!frame) {
            av_frame_free(&inpicref);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(frame, inpicref);
        frame->pts = ((s->start_time == AV_NOPTS_VALUE) ? 0 : s->start_time) +
                     av_rescale(outlink->frame_count, s->ts_unit.num,
                                s->ts_unit.den);
        ret = ff_filter_frame(outlink, frame);
    }
    av_frame_free(&inpicref);
    return ret;
}
