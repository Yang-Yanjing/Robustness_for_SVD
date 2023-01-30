}
static int OutControl(sout_access_out_t *p_access, int i_query, va_list args)
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    VLC_UNUSED(p_sys);
    switch (i_query) {
    case ACCESS_OUT_CONTROLS_PACE: {
        bool *pb = va_arg(args, bool *);
        //*pb = strcmp(p_access->psz_access, "stream");
        *pb = false;
        break;
    }
    default:
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
