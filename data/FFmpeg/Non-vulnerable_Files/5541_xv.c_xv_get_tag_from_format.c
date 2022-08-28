static int xv_get_tag_from_format(enum AVPixelFormat format)
{
    const XVTagFormatMap *m = tag_codec_map;
    int i;
    for (i = 0; m->tag; m = &tag_codec_map[++i]) {
        if (m->format == format)
            return m->tag;
    }
    return 0;
}
