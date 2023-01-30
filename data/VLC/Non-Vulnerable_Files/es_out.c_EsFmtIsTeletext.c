}
static inline bool EsFmtIsTeletext( const es_format_t *p_fmt )
{
    return p_fmt->i_cat == SPU_ES && p_fmt->i_codec == VLC_CODEC_TELETEXT;
}
