 ****************************************************************************/
static block_t *Packetize(decoder_t *p_dec, block_t **pp_block)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    return packetizer_Packetize(&p_sys->packetizer, pp_block);
}
