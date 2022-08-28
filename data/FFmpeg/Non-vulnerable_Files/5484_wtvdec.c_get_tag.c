static void get_tag(AVFormatContext *s, AVIOContext *pb, const char *key, int type, int length)
{
    int buf_size;
    char *buf;
    if (!strcmp(key, "WM/MediaThumbType")) {
        avio_skip(pb, length);
        return;
    }
    buf_size = FFMAX(2*length, LEN_PRETTY_GUID) + 1;
    buf = av_malloc(buf_size);
    if (!buf)
        return;
    if (type == 0 && length == 4) {
        snprintf(buf, buf_size, "%u", avio_rl32(pb));
    } else if (type == 1) {
        avio_get_str16le(pb, length, buf, buf_size);
        if (!strlen(buf)) {
           av_free(buf);
           return;
        }
    } else if (type == 3 && length == 4) {
        strcpy(buf, avio_rl32(pb) ? "true" : "false");
    } else if (type == 4 && length == 8) {
        int64_t num = avio_rl64(pb);
        if (!strcmp(key, "WM/EncodingTime") ||
            !strcmp(key, "WM/MediaOriginalBroadcastDateTime")) {
            if (filetime_to_iso8601(buf, buf_size, num) < 0) {
                av_free(buf);
                return;
            }
        } else if (!strcmp(key, "WM/WMRVEncodeTime") ||
                   !strcmp(key, "WM/WMRVEndTime")) {
            if (crazytime_to_iso8601(buf, buf_size, num) < 0) {
                av_free(buf);
                return;
            }
        } else if (!strcmp(key, "WM/WMRVExpirationDate")) {
            if (oledate_to_iso8601(buf, buf_size, num) < 0 ) {
                av_free(buf);
                return;
            }
        } else if (!strcmp(key, "WM/WMRVBitrate"))
            snprintf(buf, buf_size, "%f", av_int2double(num));
        else
            snprintf(buf, buf_size, "%"PRIi64, num);
    } else if (type == 5 && length == 2) {
        snprintf(buf, buf_size, "%u", avio_rl16(pb));
    } else if (type == 6 && length == 16) {
        ff_asf_guid guid;
        avio_read(pb, guid, 16);
        snprintf(buf, buf_size, PRI_PRETTY_GUID, ARG_PRETTY_GUID(guid));
    } else if (type == 2 && !strcmp(key, "WM/Picture")) {
        get_attachment(s, pb, length);
        av_freep(&buf);
        return;
    } else {
        av_freep(&buf);
        av_log(s, AV_LOG_WARNING, "unsupported metadata entry; key:%s, type:%d, length:0x%x\n", key, type, length);
        avio_skip(pb, length);
        return;
    }
    av_dict_set(&s->metadata, key, buf, 0);
    av_freep(&buf);
}
