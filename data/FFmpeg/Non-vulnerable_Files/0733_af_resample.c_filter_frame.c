static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext  *ctx = inlink->dst;
    ResampleContext    *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int ret;
    if (s->avr) {
        AVFrame *out;
        int delay, nb_samples;
        
        delay      = avresample_get_delay(s->avr);
        nb_samples = avresample_get_out_samples(s->avr, in->nb_samples);
        out = ff_get_audio_buffer(outlink, nb_samples);
        if (!out) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        ret = avresample_convert(s->avr, out->extended_data, out->linesize[0],
                                 nb_samples, in->extended_data, in->linesize[0],
                                 in->nb_samples);
        if (ret <= 0) {
            av_frame_free(&out);
            if (ret < 0)
                goto fail;
        }
        av_assert0(!avresample_available(s->avr));
        if (s->resampling && s->next_pts == AV_NOPTS_VALUE) {
            if (in->pts == AV_NOPTS_VALUE) {
                av_log(ctx, AV_LOG_WARNING, "First timestamp is missing, "
                       "assuming 0.\n");
                s->next_pts = 0;
            } else
                s->next_pts = av_rescale_q(in->pts, inlink->time_base,
                                           outlink->time_base);
        }
        if (ret > 0) {
            out->nb_samples = ret;
            ret = av_frame_copy_props(out, in);
            if (ret < 0) {
                av_frame_free(&out);
                goto fail;
            }
            if (s->resampling) {
                out->sample_rate = outlink->sample_rate;
                




                if (in->pts != AV_NOPTS_VALUE && in->pts != s->next_in_pts) {
                    out->pts = av_rescale_q(in->pts, inlink->time_base,
                                                outlink->time_base) -
                                   av_rescale(delay, outlink->sample_rate,
                                              inlink->sample_rate);
                } else
                    out->pts = s->next_pts;
                s->next_pts = out->pts + out->nb_samples;
                s->next_in_pts = in->pts + in->nb_samples;
            } else
                out->pts = in->pts;
            ret = ff_filter_frame(outlink, out);
            s->got_output = 1;
        }
fail:
        av_frame_free(&in);
    } else {
        in->format = outlink->format;
        ret = ff_filter_frame(outlink, in);
        s->got_output = 1;
    }
    return ret;
}
