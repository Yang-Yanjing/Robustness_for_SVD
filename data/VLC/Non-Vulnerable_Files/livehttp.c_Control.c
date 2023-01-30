}
static int Control( sout_access_out_t *p_access, int i_query, va_list args )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    switch( i_query )
    {
        case ACCESS_OUT_CONTROLS_PACE:
        {
            bool *pb = va_arg( args, bool * );
            *pb = !p_sys->b_ratecontrol;
            //*pb = true;
            break;
        }
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
