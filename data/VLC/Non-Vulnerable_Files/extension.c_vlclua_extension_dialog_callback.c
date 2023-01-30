 **/
static int vlclua_extension_dialog_callback( vlc_object_t *p_this,
                                             char const *psz_var,
                                             vlc_value_t oldval,
                                             vlc_value_t newval,
                                             void *p_data )
{
    /* psz_var == "dialog-event" */
    ( void ) psz_var;
    ( void ) oldval;
    ( void ) p_data;
    extension_dialog_command_t *command = newval.p_address;
    assert( command != NULL );
    assert( command->p_dlg != NULL);
    extension_t *p_ext = command->p_dlg->p_sys;
    assert( p_ext != NULL );
    extension_widget_t *p_widget = command->p_data;
    switch( command->event )
    {
        case EXTENSION_EVENT_CLICK:
            assert( p_widget != NULL );
            PushCommandUnique( p_ext, CMD_CLICK, p_widget );
            break;
        case EXTENSION_EVENT_CLOSE:
            PushCommandUnique( p_ext, CMD_CLOSE );
            break;
        default:
            msg_Dbg( p_this, "Received unknown UI event %d, discarded",
                     command->event );
            break;
    }
    return VLC_SUCCESS;
}
