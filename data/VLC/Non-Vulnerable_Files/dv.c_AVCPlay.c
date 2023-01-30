}
static int AVCPlay( access_t *p_access, int phyID )
{
    access_sys_t *p_sys = p_access->p_sys;
    msg_Dbg( p_access, "send play command over Digital Video control channel" );
    if( p_sys->p_avc1394 && phyID >= 0 )
    {
        if( !avc1394_vcr_is_recording( p_sys->p_avc1394, phyID ) &&
            avc1394_vcr_is_playing( p_sys->p_avc1394, phyID ) != AVC1394_VCR_OPERAND_PLAY_FORWARD )
                avc1394_vcr_play( p_sys->p_avc1394, phyID );
    }
    return 0;
}
