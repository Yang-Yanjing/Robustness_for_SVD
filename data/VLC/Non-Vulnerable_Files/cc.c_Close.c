 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    for( int i = 0; i < p_sys->i_block; i++ )
        block_Release( p_sys->pp_block[i] );
    free( p_sys );
}
