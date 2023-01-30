 *****************************************************************************/
int Import_QTL( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    if( !demux_IsPathExtension( p_demux, ".qtl" ) )
        return VLC_EGENERIC;
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    msg_Dbg( p_demux, "using QuickTime Media Link reader" );
    return VLC_SUCCESS;
}
