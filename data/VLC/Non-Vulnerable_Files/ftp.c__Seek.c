 *****************************************************************************/
static int _Seek( vlc_object_t *p_access, access_sys_t *p_sys, uint64_t i_pos )
{
    msg_Dbg( p_access, "seeking to %"PRIu64, i_pos );
    ftp_StopStream( (vlc_object_t *)p_access, p_sys );
    if( ftp_StartStream( (vlc_object_t *)p_access, p_sys, i_pos ) < 0 )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
