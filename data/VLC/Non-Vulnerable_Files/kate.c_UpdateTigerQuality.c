}
static void UpdateTigerQuality( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = (decoder_sys_t*)p_dec->p_sys;
    CHECK_TIGER_RET( tiger_renderer_set_quality( p_sys->p_tr, p_sys->f_tiger_quality ) );
    p_sys->b_dirty = true;
}
