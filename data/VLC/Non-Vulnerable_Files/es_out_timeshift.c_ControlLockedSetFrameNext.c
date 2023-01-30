}
static int ControlLockedSetFrameNext( es_out_t *p_out )
{
    es_out_sys_t *p_sys = p_out->p_sys;
    return es_out_SetFrameNext( p_sys->p_out );
}
