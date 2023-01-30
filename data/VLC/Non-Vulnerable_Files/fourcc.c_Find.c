/* */
static entry_t Find( int i_cat, vlc_fourcc_t i_fourcc )
{
    entry_t e;
    switch( i_cat )
    {
    case VIDEO_ES:
        return Lookup( p_list_video, i_fourcc );
    case AUDIO_ES:
        return Lookup( p_list_audio, i_fourcc );
    case SPU_ES:
        return Lookup( p_list_spu, i_fourcc );
    default:
        e = Find( VIDEO_ES, i_fourcc );
        if( CreateFourcc( e.p_class ) == 0 )
            e = Find( AUDIO_ES, i_fourcc );
        if( CreateFourcc( e.p_class ) == 0 )
            e = Find( SPU_ES, i_fourcc );
        return e;
    }
}
