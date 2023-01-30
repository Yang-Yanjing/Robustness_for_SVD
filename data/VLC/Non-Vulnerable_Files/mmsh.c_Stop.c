 *****************************************************************************/
static void Stop( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    msg_Dbg( p_access, "closing stream" );
    if( p_sys->fd > 0 )
    {
        net_Close( p_sys->fd );
        p_sys->fd = -1;
    }
}
