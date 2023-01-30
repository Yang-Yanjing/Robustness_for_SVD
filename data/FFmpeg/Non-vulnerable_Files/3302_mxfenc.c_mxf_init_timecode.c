static int mxf_init_timecode(AVFormatContext *s, AVStream *st, AVRational rate)
{
    MXFContext *mxf = s->priv_data;
    AVDictionaryEntry *tcr = av_dict_get(s->metadata, "timecode", NULL, 0);
    if (!tcr)
        tcr = av_dict_get(st->metadata, "timecode", NULL, 0);
    if (tcr)
        return av_timecode_init_from_string(&mxf->tc, rate, tcr->value, s);
    else
        return av_timecode_init(&mxf->tc, rate, 0, 0, s);
}
