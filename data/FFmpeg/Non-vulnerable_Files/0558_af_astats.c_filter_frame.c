static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AudioStatsContext *s = inlink->dst->priv;
    AVDictionary **metadata = avpriv_frame_get_metadatap(buf);
    const int channels = s->nb_channels;
    const double *src;
    int i, c;
    switch (inlink->format) {
    case AV_SAMPLE_FMT_DBLP:
        for (c = 0; c < channels; c++) {
            ChannelStats *p = &s->chstats[c];
            src = (const double *)buf->extended_data[c];
            for (i = 0; i < buf->nb_samples; i++, src++)
                update_stat(s, p, *src);
        }
        break;
    case AV_SAMPLE_FMT_DBL:
        src = (const double *)buf->extended_data[0];
        for (i = 0; i < buf->nb_samples; i++) {
            for (c = 0; c < channels; c++, src++)
                update_stat(s, &s->chstats[c], *src);
        }
        break;
    }
    if (s->metadata)
        set_metadata(s, metadata);
    if (s->reset_count > 0) {
        s->nb_frames++;
        if (s->nb_frames >= s->reset_count) {
            reset_stats(s);
            s->nb_frames = 0;
        }
    }
    return ff_filter_frame(inlink->dst->outputs[0], buf);
}
