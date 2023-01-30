}
static int AVCStop( access_t *p_access, int phyID )
{
    access_sys_t *p_sys = p_access->p_sys;
    msg_Dbg( p_access, "closing Digital Video control channel" );
    if ( p_sys->p_avc1394 && phyID >= 0 )
        avc1394_vcr_stop( p_sys->p_avc1394, phyID );
    return 0;
}
