 */
static void DpbInit( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    for( int i = 0; i < DPB_COUNT; i++ )
        p_sys->p_dpb[i].p_picture = NULL;
}
