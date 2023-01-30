static void parse_legacy_attrib(AVFormatContext *s, AVIOContext *pb)
{
    ff_asf_guid guid;
    int length, type;
    while(!avio_feof(pb)) {
        char key[1024];
        ff_get_guid(pb, &guid);
        type   = avio_rl32(pb);
        length = avio_rl32(pb);
        if (!length)
            break;
        if (ff_guidcmp(&guid, ff_metadata_guid)) {
            av_log(s, AV_LOG_WARNING, "unknown guid "FF_PRI_GUID", expected metadata_guid; "
                   "remaining metadata entries ignored\n", FF_ARG_GUID(guid));
            break;
        }
        avio_get_str16le(pb, INT_MAX, key, sizeof(key));
        get_tag(s, pb, key, type, length);
    }
    ff_metadata_conv(&s->metadata, NULL, ff_asf_metadata_conv);
}
