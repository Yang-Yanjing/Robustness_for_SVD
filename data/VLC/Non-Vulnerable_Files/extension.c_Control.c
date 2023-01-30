}
static int Control( extensions_manager_t *p_mgr, int i_control, va_list args )
{
    extension_t *p_ext = NULL;
    bool *pb = NULL;
    uint16_t **ppus = NULL;
    char ***pppsz = NULL;
    int i = 0;
    switch( i_control )
    {
        case EXTENSION_ACTIVATE:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            return Activate( p_mgr, p_ext );
        case EXTENSION_DEACTIVATE:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            return Deactivate( p_mgr, p_ext );
        case EXTENSION_IS_ACTIVATED:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            pb = ( bool* ) va_arg( args, bool* );
            *pb = IsActivated( p_mgr, p_ext );
            break;
        case EXTENSION_HAS_MENU:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            pb = ( bool* ) va_arg( args, bool* );
            *pb = ( p_ext->p_sys->i_capabilities & EXT_HAS_MENU ) ? 1 : 0;
            break;
        case EXTENSION_GET_MENU:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            pppsz = ( char*** ) va_arg( args, char*** );
            ppus = ( uint16_t** ) va_arg( args, uint16_t** );
            return GetMenuEntries( p_mgr, p_ext, pppsz, ppus );
        case EXTENSION_TRIGGER_ONLY:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            pb = ( bool* ) va_arg( args, bool* );
            *pb = ( p_ext->p_sys->i_capabilities & EXT_TRIGGER_ONLY ) ? 1 : 0;
            break;
        case EXTENSION_TRIGGER:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            return TriggerExtension( p_mgr, p_ext );
        case EXTENSION_TRIGGER_MENU:
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            // GCC: 'uint16_t' is promoted to 'int' when passed through '...'
            i = ( int ) va_arg( args, int );
            return TriggerMenu( p_ext, i );
        case EXTENSION_SET_INPUT:
        {
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            input_thread_t *p_input = va_arg( args, struct input_thread_t * );
            if( !LockExtension( p_ext ) )
                return VLC_EGENERIC;
            // Change input
            input_thread_t *old = p_ext->p_sys->p_input;
            input_item_t *p_item;
            if( old )
            {
                // Untrack meta fetched events
                if( p_ext->p_sys->i_capabilities & EXT_META_LISTENER )
                {
                    p_item = input_GetItem( old );
                    vlc_event_detach( &p_item->event_manager,
                                      vlc_InputItemMetaChanged,
                                      inputItemMetaChanged,
                                      p_ext );
                    vlc_gc_decref( p_item );
                }
                vlc_object_release( old );
            }
            p_ext->p_sys->p_input = p_input ? vlc_object_hold( p_input )
                                            : p_input;
            // Tell the script the input changed
            if( p_ext->p_sys->i_capabilities & EXT_INPUT_LISTENER )
            {
                PushCommandUnique( p_ext, CMD_SET_INPUT );
            }
            // Track meta fetched events
            if( p_ext->p_sys->p_input &&
                p_ext->p_sys->i_capabilities & EXT_META_LISTENER )
            {
                p_item = input_GetItem( p_ext->p_sys->p_input );
                vlc_gc_incref( p_item );
                vlc_event_attach( &p_item->event_manager,
                                  vlc_InputItemMetaChanged,
                                  inputItemMetaChanged,
                                  p_ext );
            }
            UnlockExtension( p_ext );
            break;
        }
        case EXTENSION_PLAYING_CHANGED:
        {
            extension_t *p_ext;
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            assert( p_ext->psz_name != NULL );
            i = ( int ) va_arg( args, int );
            if( p_ext->p_sys->i_capabilities & EXT_PLAYING_LISTENER )
            {
                PushCommand( p_ext, CMD_PLAYING_CHANGED, i );
            }
            break;
        }
        case EXTENSION_META_CHANGED:
        {
            extension_t *p_ext;
            p_ext = ( extension_t* ) va_arg( args, extension_t* );
            PushCommand( p_ext, CMD_UPDATE_META );
            break;
        }
        default:
            msg_Warn( p_mgr, "Control '%d' not yet implemented in Extension",
                      i_control );
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
