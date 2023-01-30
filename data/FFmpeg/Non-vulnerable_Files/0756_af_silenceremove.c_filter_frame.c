static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    SilenceRemoveContext *s = ctx->priv;
    int i, j, threshold, ret = 0;
    int nbs, nb_samples_read, nb_samples_written;
    double *obuf, *ibuf = (double *)in->data[0];
    AVFrame *out;
    nb_samples_read = nb_samples_written = 0;
    switch (s->mode) {
    case SILENCE_TRIM:
silence_trim:
        nbs = in->nb_samples - nb_samples_read / inlink->channels;
        if (!nbs)
            break;
        for (i = 0; i < nbs; i++) {
            threshold = 0;
            for (j = 0; j < inlink->channels; j++) {
                threshold |= compute_rms(s, ibuf[j]) > s->start_threshold;
            }
            if (threshold) {
                for (j = 0; j < inlink->channels; j++) {
                    update_rms(s, *ibuf);
                    s->start_holdoff[s->start_holdoff_end++] = *ibuf++;
                    nb_samples_read++;
                }
                if (s->start_holdoff_end >= s->start_duration * inlink->channels) {
                    if (++s->start_found_periods >= s->start_periods) {
                        s->mode = SILENCE_TRIM_FLUSH;
                        goto silence_trim_flush;
                    }
                    s->start_holdoff_offset = 0;
                    s->start_holdoff_end = 0;
                }
            } else {
                s->start_holdoff_end = 0;
                for (j = 0; j < inlink->channels; j++)
                    update_rms(s, ibuf[j]);
                ibuf += inlink->channels;
                nb_samples_read += inlink->channels;
            }
        }
        break;
    case SILENCE_TRIM_FLUSH:
silence_trim_flush:
        nbs  = s->start_holdoff_end - s->start_holdoff_offset;
        nbs -= nbs % inlink->channels;
        if (!nbs)
            break;
        out = ff_get_audio_buffer(inlink, nbs / inlink->channels);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        memcpy(out->data[0], &s->start_holdoff[s->start_holdoff_offset],
               nbs * sizeof(double));
        s->start_holdoff_offset += nbs;
        ret = ff_filter_frame(outlink, out);
        if (s->start_holdoff_offset == s->start_holdoff_end) {
            s->start_holdoff_offset = 0;
            s->start_holdoff_end = 0;
            s->mode = SILENCE_COPY;
            goto silence_copy;
        }
        break;
    case SILENCE_COPY:
silence_copy:
        nbs = in->nb_samples - nb_samples_read / inlink->channels;
        if (!nbs)
            break;
        out = ff_get_audio_buffer(inlink, nbs);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        obuf = (double *)out->data[0];
        if (s->stop_periods) {
            for (i = 0; i < nbs; i++) {
                threshold = 1;
                for (j = 0; j < inlink->channels; j++)
                    threshold &= compute_rms(s, ibuf[j]) > s->stop_threshold;
                if (threshold && s->stop_holdoff_end && !s->leave_silence) {
                    s->mode = SILENCE_COPY_FLUSH;
                    flush(out, outlink, &nb_samples_written, &ret);
                    goto silence_copy_flush;
                } else if (threshold) {
                    for (j = 0; j < inlink->channels; j++) {
                        update_rms(s, *ibuf);
                        *obuf++ = *ibuf++;
                        nb_samples_read++;
                        nb_samples_written++;
                    }
                } else if (!threshold) {
                    for (j = 0; j < inlink->channels; j++) {
                        update_rms(s, *ibuf);
                        if (s->leave_silence) {
                            *obuf++ = *ibuf;
                            nb_samples_written++;
                        }
                        s->stop_holdoff[s->stop_holdoff_end++] = *ibuf++;
                        nb_samples_read++;
                    }
                    if (s->stop_holdoff_end >= s->stop_duration * inlink->channels) {
                        if (++s->stop_found_periods >= s->stop_periods) {
                            s->stop_holdoff_offset = 0;
                            s->stop_holdoff_end = 0;
                            if (!s->restart) {
                                s->mode = SILENCE_STOP;
                                flush(out, outlink, &nb_samples_written, &ret);
                                goto silence_stop;
                            } else {
                                s->stop_found_periods = 0;
                                s->start_found_periods = 0;
                                s->start_holdoff_offset = 0;
                                s->start_holdoff_end = 0;
                                clear_rms(s);
                                s->mode = SILENCE_TRIM;
                                flush(out, outlink, &nb_samples_written, &ret);
                                goto silence_trim;
                            }
                        }
                        s->mode = SILENCE_COPY_FLUSH;
                        flush(out, outlink, &nb_samples_written, &ret);
                        goto silence_copy_flush;
                    }
                }
            }
            flush(out, outlink, &nb_samples_written, &ret);
        } else {
            memcpy(obuf, ibuf, sizeof(double) * nbs * inlink->channels);
            ret = ff_filter_frame(outlink, out);
        }
        break;
    case SILENCE_COPY_FLUSH:
silence_copy_flush:
        nbs  = s->stop_holdoff_end - s->stop_holdoff_offset;
        nbs -= nbs % inlink->channels;
        if (!nbs)
            break;
        out = ff_get_audio_buffer(inlink, nbs / inlink->channels);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        memcpy(out->data[0], &s->stop_holdoff[s->stop_holdoff_offset],
               nbs * sizeof(double));
        s->stop_holdoff_offset += nbs;
        ret = ff_filter_frame(outlink, out);
        if (s->stop_holdoff_offset == s->stop_holdoff_end) {
            s->stop_holdoff_offset = 0;
            s->stop_holdoff_end = 0;
            s->mode = SILENCE_COPY;
            goto silence_copy;
        }
        break;
    case SILENCE_STOP:
silence_stop:
        break;
    }
    av_frame_free(&in);
    return ret;
}
