static int parse_MP4ESDescrTag(MP4DescrParseContext *d, int64_t off, int len)
{
    int es_id = 0;
    if (d->descr_count >= d->max_descr_count)
        return AVERROR_INVALIDDATA;
    ff_mp4_parse_es_descr(&d->pb, &es_id);
    d->active_descr = d->descr + (d->descr_count++);
    d->active_descr->es_id = es_id;
    update_offsets(&d->pb, &off, &len);
    parse_mp4_descr(d, off, len, MP4DecConfigDescrTag);
    update_offsets(&d->pb, &off, &len);
    if (len > 0)
        parse_mp4_descr(d, off, len, MP4SLDescrTag);
    d->active_descr = NULL;
    return 0;
}
