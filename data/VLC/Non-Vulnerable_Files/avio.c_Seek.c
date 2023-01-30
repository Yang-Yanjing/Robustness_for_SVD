}
static int Seek(access_t *access, uint64_t position)
{
    access_sys_t *sys = access->p_sys;
    int ret;
#ifndef EOVERFLOW
# define EOVERFLOW EFBIG
#endif
    if (position > INT64_MAX)
        ret = AVERROR(EOVERFLOW);
    else
        ret = avio_seek(sys->context, position, SEEK_SET);
    if (ret < 0) {
        msg_Err(access, "Seek to %"PRIu64" failed: %s", position,
                vlc_strerror_c(AVUNERROR(ret)));
        if (sys->size == 0 || position != sys->size)
            return VLC_EGENERIC;
    }
    access->info.i_pos = position;
    access->info.b_eof = false;
    return VLC_SUCCESS;
}
