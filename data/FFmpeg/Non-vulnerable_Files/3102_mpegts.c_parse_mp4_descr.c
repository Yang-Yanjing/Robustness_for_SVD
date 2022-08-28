static int parse_mp4_descr(MP4DescrParseContext *d, int64_t off, int len,
                           int target_tag)
{
    int tag;
    int len1 = ff_mp4_read_descr(d->s, &d->pb, &tag);
    update_offsets(&d->pb, &off, &len);
    if (len < 0 || len1 > len || len1 <= 0) {
        av_log(d->s, AV_LOG_ERROR,
               "Tag %x length violation new length %d bytes remaining %d\n",
               tag, len1, len);
        return AVERROR_INVALIDDATA;
    }
    if (d->level++ >= MAX_LEVEL) {
        av_log(d->s, AV_LOG_ERROR, "Maximum MP4 descriptor level exceeded\n");
        goto done;
    }
    if (target_tag && tag != target_tag) {
        av_log(d->s, AV_LOG_ERROR, "Found tag %x expected %x\n", tag,
               target_tag);
        goto done;
    }
    switch (tag) {
    case MP4IODescrTag:
        parse_MP4IODescrTag(d, off, len1);
        break;
    case MP4ODescrTag:
        parse_MP4ODescrTag(d, off, len1);
        break;
    case MP4ESDescrTag:
        parse_MP4ESDescrTag(d, off, len1);
        break;
    case MP4DecConfigDescrTag:
        parse_MP4DecConfigDescrTag(d, off, len1);
        break;
    case MP4SLDescrTag:
        parse_MP4SLDescrTag(d, off, len1);
        break;
    }
done:
    d->level--;
    avio_seek(&d->pb, off + len1, SEEK_SET);
    return 0;
}
