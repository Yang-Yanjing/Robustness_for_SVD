}
static int InputEvent( vlc_object_t *p_this, char const *psz_cmd,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(psz_cmd);
    VLC_UNUSED(oldval);
    input_thread_t *p_input = (input_thread_t*)p_this;
    intf_thread_t *p_intf = p_data;
    switch( newval.i_int )
    {
    case INPUT_EVENT_STATE:
    case INPUT_EVENT_DEAD:
        StateChanged( p_intf, p_input );
        break;
    case INPUT_EVENT_RATE:
        RateChanged( p_intf, p_input );
        break;
    case INPUT_EVENT_POSITION:
        PositionChanged( p_intf, p_input );
        break;
    case INPUT_EVENT_CACHE:
        CacheChanged( p_intf );
        break;
    default:
        break;
    }
    return VLC_SUCCESS;
}
