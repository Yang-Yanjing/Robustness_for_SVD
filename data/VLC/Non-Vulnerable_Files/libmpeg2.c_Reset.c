 *****************************************************************************/
static void Reset( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    cc_Flush( &p_sys->cc );
    mpeg2_reset( p_sys->p_mpeg2dec, 0 );
    DpbClean( p_dec );
}
