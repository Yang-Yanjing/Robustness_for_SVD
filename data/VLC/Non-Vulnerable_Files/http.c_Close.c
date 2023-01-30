 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_access_out_t       *p_access = (sout_access_out_t*)p_this;
    sout_access_out_sys_t   *p_sys = p_access->p_sys;
    httpd_StreamDelete( p_sys->p_httpd_stream );
    httpd_HostDelete( p_sys->p_httpd_host );
    free( p_sys->p_header );
    msg_Dbg( p_access, "Close" );
    free( p_sys );
}
