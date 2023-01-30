static int mov_write_track_metadata(AVIOContext *pb, AVStream *st,
                                    const char *tag, const char *str)
{
    int64_t pos = avio_tell(pb);
    AVDictionaryEntry *t = av_dict_get(st->metadata, str, NULL, 0);
    if (!t || !utf8len(t->value))
        return 0;
    avio_wb32(pb, 0);   
    ffio_wfourcc(pb, tag); 
    avio_write(pb, t->value, strlen(t->value)); 
    return update_size(pb, pos);
}
