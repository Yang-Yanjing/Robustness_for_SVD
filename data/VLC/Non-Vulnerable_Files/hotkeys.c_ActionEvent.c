 *****************************************************************************/
static int ActionEvent( vlc_object_t *libvlc, char const *psz_var,
                        vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_data;
    (void)libvlc;
    (void)psz_var;
    (void)oldval;
    return PutAction( p_intf, newval.i_int );
}
