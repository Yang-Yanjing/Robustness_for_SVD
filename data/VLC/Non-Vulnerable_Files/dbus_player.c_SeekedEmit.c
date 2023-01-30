 *****************************************************************************/
int SeekedEmit( intf_thread_t * p_intf )
{
    if( p_intf->p_sys->b_dead )
        return VLC_SUCCESS;
    SeekedSignal( p_intf->p_sys->p_conn, p_intf );
    return VLC_SUCCESS;
}
