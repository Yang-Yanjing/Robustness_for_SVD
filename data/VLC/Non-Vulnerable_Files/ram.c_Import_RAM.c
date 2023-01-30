 */
int Import_RAM( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    const uint8_t *p_peek;
    if(! demux_IsPathExtension( p_demux, ".ram" ) ||
         demux_IsPathExtension( p_demux, ".rm" ) )
        return VLC_EGENERIC;
    /* Many Real Media Files are misdetected */
    if( stream_Peek( p_demux->s, &p_peek, 4 ) < 4 )
        return VLC_EGENERIC;
    if( !memcmp( p_peek, ".ra", 3 ) || !memcmp( p_peek, ".RMF", 4 ) )
    {
        return VLC_EGENERIC;
    }
    STANDARD_DEMUX_INIT_MSG( "found valid RAM playlist" );
    p_demux->p_sys->psz_prefix = FindPrefix( p_demux );
    return VLC_SUCCESS;
}
