}
static int Control( sout_access_out_t *p_access, int i_query, va_list args )
{
    (void)p_access;
    switch( i_query )
    {
        case ACCESS_OUT_CONTROLS_PACE:
            *va_arg( args, bool * ) = false;
            break;
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
