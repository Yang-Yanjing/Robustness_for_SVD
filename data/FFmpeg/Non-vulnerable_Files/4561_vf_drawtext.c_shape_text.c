static int shape_text(AVFilterContext *ctx)
{
    DrawTextContext *s = ctx->priv;
    uint8_t *tmp;
    int ret = AVERROR(ENOMEM);
    static const FriBidiFlags flags = FRIBIDI_FLAGS_DEFAULT |
                                      FRIBIDI_FLAGS_ARABIC;
    FriBidiChar *unicodestr = NULL;
    FriBidiStrIndex len;
    FriBidiParType direction = FRIBIDI_PAR_LTR;
    FriBidiStrIndex line_start = 0;
    FriBidiStrIndex line_end = 0;
    FriBidiLevel *embedding_levels = NULL;
    FriBidiArabicProp *ar_props = NULL;
    FriBidiCharType *bidi_types = NULL;
    FriBidiStrIndex i,j;
    len = strlen(s->text);
    if (!(unicodestr = av_malloc_array(len, sizeof(*unicodestr)))) {
        goto out;
    }
    len = fribidi_charset_to_unicode(FRIBIDI_CHAR_SET_UTF8,
                                     s->text, len, unicodestr);
    bidi_types = av_malloc_array(len, sizeof(*bidi_types));
    if (!bidi_types) {
        goto out;
    }
    fribidi_get_bidi_types(unicodestr, len, bidi_types);
    embedding_levels = av_malloc_array(len, sizeof(*embedding_levels));
    if (!embedding_levels) {
        goto out;
    }
    if (!fribidi_get_par_embedding_levels(bidi_types, len, &direction,
                                          embedding_levels)) {
        goto out;
    }
    ar_props = av_malloc_array(len, sizeof(*ar_props));
    if (!ar_props) {
        goto out;
    }
    fribidi_get_joining_types(unicodestr, len, ar_props);
    fribidi_join_arabic(bidi_types, len, embedding_levels, ar_props);
    fribidi_shape(flags, embedding_levels, len, ar_props, unicodestr);
    for (line_end = 0, line_start = 0; line_end < len; line_end++) {
        if (is_newline(unicodestr[line_end]) || line_end == len - 1) {
            if (!fribidi_reorder_line(flags, bidi_types,
                                      line_end - line_start + 1, line_start,
                                      direction, embedding_levels, unicodestr,
                                      NULL)) {
                goto out;
            }
            line_start = line_end + 1;
        }
    }
    
    for (i = 0, j = 0; i < len; i++)
        if (unicodestr[i] != FRIBIDI_CHAR_FILL)
            unicodestr[j++] = unicodestr[i];
    len = j;
    if (!(tmp = av_realloc(s->text, (len * 4 + 1) * sizeof(*s->text)))) {
        
        goto out;
    }
    s->text = tmp;
    len = fribidi_unicode_to_charset(FRIBIDI_CHAR_SET_UTF8,
                                     unicodestr, len, s->text);
    ret = 0;
out:
    av_free(unicodestr);
    av_free(embedding_levels);
    av_free(ar_props);
    av_free(bidi_types);
    return ret;
}
