 *****************************************************************************/
static int OpenDemux( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    char * psz_name = p_demux->psz_location;
    p_demux->p_sys = NULL;
    /* Check for a "vlc://nop" command */
    if( !strcasecmp( psz_name, "nop" ) )
    {
nop:
        msg_Info( p_demux, "command `nop'" );
        p_demux->pf_demux = DemuxNoOp;
        p_demux->pf_control = DemuxControl;
        return VLC_SUCCESS;
    }
    /* Check for a "vlc://quit" command */
    if( !strcasecmp( psz_name, "quit" ) )
    {
        msg_Info( p_demux, "command `quit'" );
        p_demux->pf_demux = DemuxNoOp;
        p_demux->pf_control = DemuxControl;
        libvlc_Quit( p_demux->p_libvlc );
        return VLC_SUCCESS;
    }
    if( !strcasecmp( psz_name, "pause" ) )
    {
        msg_Info( p_demux, "command `pause'" );
        p_demux->pf_demux = DemuxHold;
        p_demux->pf_control = DemuxControl;
        return VLC_SUCCESS;
    }
    /* Check for a "vlc://pause:***" command */
    if( !strncasecmp( psz_name, "pause:", 6 ) )
    {
        double f = us_atof( psz_name + 6 );
        mtime_t length = f * CLOCK_FREQ;
        msg_Info( p_demux, "command `pause %f'", f );
        if( length == 0 )
            goto nop; /* avoid division by zero */
        demux_sys_t *p_sys = malloc( sizeof( *p_sys ) );
        if( p_sys == NULL )
            return VLC_ENOMEM;
        p_sys->end = mdate() + length;
        p_sys->length = length;
        p_demux->p_sys = p_sys;
        p_demux->pf_demux = DemuxPause;
        p_demux->pf_control = ControlPause;
        return VLC_SUCCESS;
    }
    msg_Err( p_demux, "unknown command `%s'", psz_name );
    return VLC_EGENERIC;
}
