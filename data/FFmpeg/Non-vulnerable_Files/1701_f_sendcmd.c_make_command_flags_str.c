static inline char *make_command_flags_str(AVBPrint *pbuf, int flags)
{
    static const char * const flag_strings[] = { "enter", "leave" };
    int i, is_first = 1;
    av_bprint_init(pbuf, 0, AV_BPRINT_SIZE_AUTOMATIC);
    for (i = 0; i < FF_ARRAY_ELEMS(flag_strings); i++) {
        if (flags & 1<<i) {
            if (!is_first)
                av_bprint_chars(pbuf, '+', 1);
            av_bprintf(pbuf, "%s", flag_strings[i]);
            is_first = 0;
        }
    }
    return pbuf->str;
}
