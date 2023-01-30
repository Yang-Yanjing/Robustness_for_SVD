static void set_language(AVFormatContext *s, const char *rfc1766, AVDictionary **met)
{
    
    if (rfc1766 && strlen(rfc1766) > 1) {
        const char primary_tag[3] = { rfc1766[0], rfc1766[1], '\0' }; 
        const char *iso6392       = av_convert_lang_to(primary_tag,
                                                       AV_LANG_ISO639_2_BIBL);
        if (iso6392)
            if (av_dict_set(met, "language", iso6392, 0) < 0)
                av_log(s, AV_LOG_WARNING, "av_dict_set failed.\n");
    }
}
