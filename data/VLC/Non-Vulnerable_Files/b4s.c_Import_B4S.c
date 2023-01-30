 *****************************************************************************/
int Import_B4S( vlc_object_t *p_this )
{
    demux_t *demux = (demux_t *)p_this;
    if( !demux_IsPathExtension( demux, ".b4s" )
     && !demux_IsForced( demux, "b4s-open" ) )
        return VLC_EGENERIC;
    demux->pf_demux = Demux;
    demux->pf_control = Control;
    return VLC_SUCCESS;
}
