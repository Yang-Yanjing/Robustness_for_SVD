 *****************************************************************************/
int Import_Shoutcast( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    if( !demux_IsForced( p_demux, "shout-winamp" ) )
        return VLC_EGENERIC;
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    msg_Dbg( p_demux, "using shoutcast playlist reader" );
    return VLC_SUCCESS;
}
