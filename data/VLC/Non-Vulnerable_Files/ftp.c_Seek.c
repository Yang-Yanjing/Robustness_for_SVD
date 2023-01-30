}
static int Seek( access_t *p_access, uint64_t i_pos )
{
    int val = _Seek( (vlc_object_t *)p_access, p_access->p_sys, i_pos );
    if( val )
        return val;
    p_access->info.b_eof = false;
    p_access->info.i_pos = i_pos;
    return VLC_SUCCESS;
}
