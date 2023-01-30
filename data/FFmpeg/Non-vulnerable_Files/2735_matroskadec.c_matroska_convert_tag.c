static void matroska_convert_tag(AVFormatContext *s, EbmlList *list,
                                 AVDictionary **metadata, char *prefix)
{
    MatroskaTag *tags = list->elem;
    char key[1024];
    int i;
    for (i = 0; i < list->nb_elem; i++) {
        const char *lang = tags[i].lang &&
                           strcmp(tags[i].lang, "und") ? tags[i].lang : NULL;
        if (!tags[i].name) {
            av_log(s, AV_LOG_WARNING, "Skipping invalid tag with no TagName.\n");
            continue;
        }
        if (prefix)
            snprintf(key, sizeof(key), "%s/%s", prefix, tags[i].name);
        else
            av_strlcpy(key, tags[i].name, sizeof(key));
        if (tags[i].def || !lang) {
            av_dict_set(metadata, key, tags[i].string, 0);
            if (tags[i].sub.nb_elem)
                matroska_convert_tag(s, &tags[i].sub, metadata, key);
        }
        if (lang) {
            av_strlcat(key, "-", sizeof(key));
            av_strlcat(key, lang, sizeof(key));
            av_dict_set(metadata, key, tags[i].string, 0);
            if (tags[i].sub.nb_elem)
                matroska_convert_tag(s, &tags[i].sub, metadata, key);
        }
    }
    ff_metadata_conv(metadata, NULL, ff_mkv_metadata_conv);
}
