static int mkv_write_tag(AVFormatContext *s, AVDictionary *m, unsigned int elementid,
                         unsigned int uid, ebml_master *tags)
{
    ebml_master tag;
    int ret;
    AVDictionaryEntry *t = NULL;
    ret = mkv_write_tag_targets(s, elementid, uid, tags, &tag);
    if (ret < 0)
        return ret;
    while ((t = av_dict_get(m, "", t, AV_DICT_IGNORE_SUFFIX))) {
        if (av_strcasecmp(t->key, "title") &&
            av_strcasecmp(t->key, "stereo_mode") &&
            av_strcasecmp(t->key, "encoding_tool")) {
            ret = mkv_write_simpletag(s->pb, t);
            if (ret < 0)
                return ret;
        }
    }
    end_ebml_master(s->pb, tag);
    return 0;
}
