}
bool es_format_IsSimilar( const es_format_t *p_fmt1, const es_format_t *p_fmt2 )
{
    if( p_fmt1->i_cat != p_fmt2->i_cat ||
        vlc_fourcc_GetCodec( p_fmt1->i_cat, p_fmt1->i_codec ) !=
        vlc_fourcc_GetCodec( p_fmt2->i_cat, p_fmt2->i_codec ) )
        return false;
    switch( p_fmt1->i_cat )
    {
    case AUDIO_ES:
    {
        audio_format_t a1 = p_fmt1->audio;
        audio_format_t a2 = p_fmt2->audio;
        if( a1.i_format && a2.i_format && a1.i_format != a2.i_format )
            return false;
        if( a1.i_rate != a2.i_rate ||
            a1.i_physical_channels != a2.i_physical_channels ||
            a1.i_original_channels != a2.i_original_channels )
            return false;
        return true;
    }
    case VIDEO_ES:
    {
        video_format_t v1 = p_fmt1->video;
        video_format_t v2 = p_fmt2->video;
        if( !v1.i_chroma )
            v1.i_chroma = vlc_fourcc_GetCodec( p_fmt1->i_cat, p_fmt1->i_codec );
        if( !v2.i_chroma )
            v2.i_chroma = vlc_fourcc_GetCodec( p_fmt2->i_cat, p_fmt2->i_codec );
        return video_format_IsSimilar( &p_fmt1->video, &p_fmt2->video );
    }
    case SPU_ES:
    default:
        return true;
    }
}
