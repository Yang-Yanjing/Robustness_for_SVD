static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext  *ctx = inlink->dst;
    ASyncContext       *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    int nb_channels = av_get_channel_layout_nb_channels(buf->channel_layout);
    int64_t pts = (buf->pts == AV_NOPTS_VALUE) ? buf->pts :
                  av_rescale_q(buf->pts, inlink->time_base, outlink->time_base);
    int out_size, ret;
    int64_t delta;
    int64_t new_pts;
    
    if (s->pts == AV_NOPTS_VALUE || pts == AV_NOPTS_VALUE) {
        if (pts != AV_NOPTS_VALUE) {
            s->pts = pts - get_delay(s);
        }
        return write_to_fifo(s, buf);
    }
    if (s->first_pts != AV_NOPTS_VALUE) {
        handle_trimming(ctx);
        if (!avresample_available(s->avr))
            return write_to_fifo(s, buf);
    }
    

    delta    = pts - s->pts - get_delay(s);
    out_size = avresample_available(s->avr);
    if (llabs(delta) > s->min_delta ||
        (s->first_frame && delta && s->first_pts != AV_NOPTS_VALUE)) {
        av_log(ctx, AV_LOG_VERBOSE, "Discontinuity - %"PRId64" samples.\n", delta);
        out_size = av_clipl_int32((int64_t)out_size + delta);
    } else {
        if (s->resample) {
            
            int delay = get_delay(s);
            int comp = s->comp + av_clip(delta * inlink->sample_rate / delay,
                                         -s->max_comp, s->max_comp);
            if (comp != s->comp) {
                av_log(ctx, AV_LOG_VERBOSE, "Compensating %d samples per second.\n", comp);
                if (avresample_set_compensation(s->avr, comp, inlink->sample_rate) == 0) {
                    s->comp = comp;
                }
            }
        }
        
        pts -= delta;
        delta = 0;
    }
    if (out_size > 0) {
        AVFrame *buf_out = ff_get_audio_buffer(outlink, out_size);
        if (!buf_out) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        if (s->first_frame && delta > 0) {
            int planar = av_sample_fmt_is_planar(buf_out->format);
            int planes = planar ?  nb_channels : 1;
            int block_size = av_get_bytes_per_sample(buf_out->format) *
                             (planar ? 1 : nb_channels);
            int ch;
            av_samples_set_silence(buf_out->extended_data, 0, delta,
                                   nb_channels, buf->format);
            for (ch = 0; ch < planes; ch++)
                buf_out->extended_data[ch] += delta * block_size;
            avresample_read(s->avr, buf_out->extended_data, out_size);
            for (ch = 0; ch < planes; ch++)
                buf_out->extended_data[ch] -= delta * block_size;
        } else {
            avresample_read(s->avr, buf_out->extended_data, out_size);
            if (delta > 0) {
                av_samples_set_silence(buf_out->extended_data, out_size - delta,
                                       delta, nb_channels, buf->format);
            }
        }
        buf_out->pts = s->pts;
        ret = ff_filter_frame(outlink, buf_out);
        if (ret < 0)
            goto fail;
        s->got_output = 1;
    } else if (avresample_available(s->avr)) {
        av_log(ctx, AV_LOG_WARNING, "Non-monotonous timestamps, dropping "
               "whole buffer.\n");
    }
    
    avresample_read(s->avr, NULL, avresample_available(s->avr));
    new_pts = pts - avresample_get_delay(s->avr);
    
    if (new_pts > s->pts) {
        s->pts = new_pts;
        ret = avresample_convert(s->avr, NULL, 0, 0, buf->extended_data,
                                 buf->linesize[0], buf->nb_samples);
    } else {
        av_log(ctx, AV_LOG_WARNING, "Non-monotonous timestamps, dropping "
               "whole buffer.\n");
        ret = 0;
    }
    s->first_frame = 0;
fail:
    av_frame_free(&buf);
    return ret;
}
