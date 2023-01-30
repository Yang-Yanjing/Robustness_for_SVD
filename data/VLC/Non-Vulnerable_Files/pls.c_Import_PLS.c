 *****************************************************************************/
int Import_PLS( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    const uint8_t *p_peek;
    CHECK_PEEK( p_peek, 10 );
    if( POKE( p_peek, "[playlist]", 10 ) || POKE( p_peek, "[Reference]", 10 ) ||
        demux_IsPathExtension( p_demux, ".pls" )   || demux_IsForced( p_demux, "pls" ) )
    {
        ;
    }
    else return VLC_EGENERIC;
    STANDARD_DEMUX_INIT_MSG(  "found valid PLS playlist file");
    p_demux->p_sys->psz_prefix = FindPrefix( p_demux );
    return VLC_SUCCESS;
}
