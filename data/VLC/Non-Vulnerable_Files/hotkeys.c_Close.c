 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    var_DelCallback( p_intf->p_libvlc, "key-action", ActionEvent, p_intf );
    /* Destroy structure */
    free( p_sys );
}
