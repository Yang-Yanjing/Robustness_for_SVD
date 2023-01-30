}
static bool ControlIsSeekRequest( int i_type )
{
    switch( i_type )
    {
    case INPUT_CONTROL_SET_POSITION:
    case INPUT_CONTROL_SET_TIME:
    case INPUT_CONTROL_SET_TITLE:
    case INPUT_CONTROL_SET_TITLE_NEXT:
    case INPUT_CONTROL_SET_TITLE_PREV:
    case INPUT_CONTROL_SET_SEEKPOINT:
    case INPUT_CONTROL_SET_SEEKPOINT_NEXT:
    case INPUT_CONTROL_SET_SEEKPOINT_PREV:
    case INPUT_CONTROL_SET_BOOKMARK:
    case INPUT_CONTROL_NAV_ACTIVATE:
    case INPUT_CONTROL_NAV_UP:
    case INPUT_CONTROL_NAV_DOWN:
    case INPUT_CONTROL_NAV_LEFT:
    case INPUT_CONTROL_NAV_RIGHT:
        return true;
    default:
        return false;
    }
}
