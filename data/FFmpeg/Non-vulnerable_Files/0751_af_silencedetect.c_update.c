static av_always_inline void update(SilenceDetectContext *s, AVFrame *insamples,
                                    int is_silence, int64_t nb_samples_notify,
                                    AVRational time_base)
{
    if (is_silence) {
        if (!s->start) {
            s->nb_null_samples++;
            if (s->nb_null_samples >= nb_samples_notify) {
                s->start = insamples->pts - (int64_t)(s->duration / av_q2d(time_base) + .5);
                av_dict_set(&insamples->metadata, "lavfi.silence_start",
                            av_ts2timestr(s->start, &time_base), 0);
                av_log(s, AV_LOG_INFO, "silence_start: %s\n",
                       get_metadata_val(insamples, "lavfi.silence_start"));
            }
        }
    } else {
        if (s->start) {
            av_dict_set(&insamples->metadata, "lavfi.silence_end",
                        av_ts2timestr(insamples->pts, &time_base), 0);
            av_dict_set(&insamples->metadata, "lavfi.silence_duration",
                        av_ts2timestr(insamples->pts - s->start, &time_base), 0);
            av_log(s, AV_LOG_INFO,
                   "silence_end: %s | silence_duration: %s\n",
                   get_metadata_val(insamples, "lavfi.silence_end"),
                   get_metadata_val(insamples, "lavfi.silence_duration"));
        }
        s->nb_null_samples = s->start = 0;
    }
}
