/* */
void bd_clpi_stc_Parse( bd_clpi_stc_t *p_stc, bs_t *s )
{
    p_stc->i_pcr_pid = bs_read( s, 16 );
    p_stc->i_packet = bs_read( s, 32 );
    p_stc->i_start = bs_read( s, 32 );
    p_stc->i_end = bs_read( s, 32 );
}
