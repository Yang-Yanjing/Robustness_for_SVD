/* Close: quit the module */
static void Close( vlc_object_t* p_this )
{
    access_t*   p_access = (access_t*)p_this;
    access_sys_t* p_sys = p_access->p_sys;
    libssh2_sftp_close_handle( p_sys->file );
    libssh2_sftp_shutdown( p_sys->sftp_session );
    libssh2_session_free( p_sys->ssh_session );
    net_Close( p_sys->i_socket );
    free( p_sys );
}
