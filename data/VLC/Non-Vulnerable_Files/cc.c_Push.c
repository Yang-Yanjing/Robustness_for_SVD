 *****************************************************************************/
static void Push( decoder_t *p_dec, block_t *p_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->i_block >= CC_MAX_REORDER_SIZE )
    {
        msg_Warn( p_dec, "Trashing a CC entry" );
        memmove( &p_sys->pp_block[0], &p_sys->pp_block[1], sizeof(*p_sys->pp_block) * (CC_MAX_REORDER_SIZE-1) );
        p_sys->i_block--;
    }
    p_sys->pp_block[p_sys->i_block++] = p_block;
}
