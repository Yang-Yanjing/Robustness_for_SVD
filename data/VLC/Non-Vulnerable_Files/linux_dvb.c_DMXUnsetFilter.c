 *****************************************************************************/
int DMXUnsetFilter( access_t * p_access, int i_fd )
{
    if( ioctl( i_fd, DMX_STOP ) < 0 )
    {
        msg_Err( p_access, "stopping demux failed: %s",
                 vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_access, "DMXUnsetFilter: closing demux %d", i_fd );
    close( i_fd );
    return VLC_SUCCESS;
}
