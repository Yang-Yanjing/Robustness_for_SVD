}
static int OutSeek(sout_access_out_t *p_access, off_t i_pos)
{
    sout_access_out_sys_t *sys = p_access->p_sys;
    if (avio_seek(sys->context, i_pos, SEEK_SET) < 0)
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
