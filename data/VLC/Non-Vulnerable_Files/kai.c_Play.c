 *****************************************************************************/
static void Play (audio_output_t *p_aout, block_t *block)
{
    aout_sys_t *p_sys = p_aout->sys;
    kaiPlay( p_sys->hkai );
    WriteBuffer( p_aout, block->p_buffer, block->i_buffer );
    block_Release( block );
}
