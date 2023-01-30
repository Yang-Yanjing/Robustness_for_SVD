}
static int ControlLockedGetEmpty( es_out_t *p_out, bool *pb_empty )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    if( p_sys->b_delayed && TsHasCmd( p_sys->p_ts ) )
        *pb_empty = false;
    else
        *pb_empty = es_out_GetEmpty( p_sys->p_out );
    return VLC_SUCCESS;
}
