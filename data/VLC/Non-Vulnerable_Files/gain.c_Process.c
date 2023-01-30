 *****************************************************************************/
static block_t *Process( filter_t *p_filter, block_t *p_block )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    p_sys->volume.amplify( &p_sys->volume, p_block, p_sys->f_gain );
    return p_block;
}
