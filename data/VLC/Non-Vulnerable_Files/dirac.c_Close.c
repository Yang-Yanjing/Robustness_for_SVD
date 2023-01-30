 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_BytestreamRelease( &p_sys->bytestream );
    if( p_sys->p_outqueue )
        block_ChainRelease( p_sys->p_outqueue );
    if( p_sys->p_eu )
        block_ChainRelease( p_sys->p_eu );
    free( p_sys );
}
