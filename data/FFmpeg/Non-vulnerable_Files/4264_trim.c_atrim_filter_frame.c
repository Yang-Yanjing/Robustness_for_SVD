static int atrim_filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    TrimContext       *s = ctx->priv;
    int64_t start_sample, end_sample;
    int64_t pts;
    int drop;
    
    if (s->eof) {
        av_frame_free(&frame);
        return 0;
    }
    if (frame->pts != AV_NOPTS_VALUE)
        pts = av_rescale_q(frame->pts, inlink->time_base,
                           (AVRational){ 1, inlink->sample_rate });
    else
        pts = s->next_pts;
    s->next_pts = pts + frame->nb_samples;
    
    if (s->start_sample < 0 && s->start_pts == AV_NOPTS_VALUE) {
        start_sample = 0;
    } else {
        drop = 1;
        start_sample = frame->nb_samples;
        if (s->start_sample >= 0 &&
            s->nb_samples + frame->nb_samples > s->start_sample) {
            drop         = 0;
            start_sample = FFMIN(start_sample, s->start_sample - s->nb_samples);
        }
        if (s->start_pts != AV_NOPTS_VALUE && pts != AV_NOPTS_VALUE &&
            pts + frame->nb_samples > s->start_pts) {
            drop = 0;
            start_sample = FFMIN(start_sample, s->start_pts - pts);
        }
        if (drop)
            goto drop;
    }
    if (s->first_pts == AV_NOPTS_VALUE)
        s->first_pts = pts + start_sample;
    
    if (s->end_sample == INT64_MAX && s->end_pts == AV_NOPTS_VALUE && !s->duration_tb) {
        end_sample = frame->nb_samples;
    } else {
        drop       = 1;
        end_sample = 0;
        if (s->end_sample != INT64_MAX &&
            s->nb_samples < s->end_sample) {
            drop       = 0;
            end_sample = FFMAX(end_sample, s->end_sample - s->nb_samples);
        }
        if (s->end_pts != AV_NOPTS_VALUE && pts != AV_NOPTS_VALUE &&
            pts < s->end_pts) {
            drop       = 0;
            end_sample = FFMAX(end_sample, s->end_pts - pts);
        }
        if (s->duration_tb && pts - s->first_pts < s->duration_tb) {
            drop       = 0;
            end_sample = FFMAX(end_sample, s->first_pts + s->duration_tb - pts);
        }
        if (drop) {
            s->eof = inlink->closed = 1;
            goto drop;
        }
    }
    s->nb_samples += frame->nb_samples;
    start_sample   = FFMAX(0, start_sample);
    end_sample     = FFMIN(frame->nb_samples, end_sample);
    av_assert0(start_sample < end_sample || (start_sample == end_sample && !frame->nb_samples));
    if (start_sample) {
        AVFrame *out = ff_get_audio_buffer(ctx->outputs[0], end_sample - start_sample);
        if (!out) {
            av_frame_free(&frame);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, frame);
        av_samples_copy(out->extended_data, frame->extended_data, 0, start_sample,
                        out->nb_samples, inlink->channels,
                        frame->format);
        if (out->pts != AV_NOPTS_VALUE)
            out->pts += av_rescale_q(start_sample, (AVRational){ 1, out->sample_rate },
                                     inlink->time_base);
        av_frame_free(&frame);
        frame = out;
    } else
        frame->nb_samples = end_sample;
    return ff_filter_frame(ctx->outputs[0], frame);
drop:
    s->nb_samples += frame->nb_samples;
    av_frame_free(&frame);
    return 0;
}
