 *****************************************************************************/
static int Open ( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    /* Allocate instance and initialize some members */
    intf_sys_t *p_sys = p_intf->p_sys = malloc( sizeof( intf_sys_t ) );
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    // Configure the module
    vlc_mutex_init( &p_sys->lock );
    p_sys->p_input = NULL;
    p_sys->p_vout = NULL;
    p_sys->b_button_pressed = false;
    p_sys->i_threshold = var_InheritInteger( p_intf, "gestures-threshold" );
    // Choose the tight button to use
    char *psz_button = var_InheritString( p_intf, "gestures-button" );
    if( psz_button && !strcmp( psz_button, "left" ) )
        p_sys->i_button_mask = 1;
    else if( psz_button && !strcmp( psz_button, "middle" ) )
        p_sys->i_button_mask = 2;
    else // psz_button == "right"
        p_sys->i_button_mask = 4;
    free( psz_button );
    p_sys->i_pattern = 0;
    p_sys->i_num_gestures = 0;
    var_AddCallback( pl_Get(p_intf), "input-current", PlaylistEvent, p_intf );
    return VLC_SUCCESS;
}
