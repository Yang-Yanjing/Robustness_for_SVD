#ifdef ENABLE_SOUT
static int OutSeek( sout_access_out_t *p_access, off_t i_pos )
{
    return _Seek( (vlc_object_t *)p_access, GET_OUT_SYS( p_access ), i_pos);
}
