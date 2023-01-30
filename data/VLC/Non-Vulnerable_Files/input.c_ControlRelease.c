}
static void ControlRelease( int i_type, vlc_value_t val )
{
    switch( i_type )
    {
    case INPUT_CONTROL_ADD_SUBTITLE:
    case INPUT_CONTROL_ADD_SLAVE:
        free( val.psz_string );
        break;
    default:
        break;
    }
}
