 ****************************************************************************/
static void PacketizeReset(void *p_private, bool b_broken)
{
    VLC_UNUSED(b_broken);
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_ChainRelease(p_sys->p_frame);
    p_sys->p_frame = NULL;
    p_sys->b_vcl = false;
}
