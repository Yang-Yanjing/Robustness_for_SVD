}
static int Control( sout_access_out_t *p_access, int i_query, va_list args )
{
    switch( i_query )
    {
        case ACCESS_OUT_CONTROLS_PACE:
        {
            bool *pb = va_arg( args, bool * );
            *pb = strcmp( p_access->psz_access, "stream" );
            break;
        }
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
