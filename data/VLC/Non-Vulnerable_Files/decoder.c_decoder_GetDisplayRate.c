 */
int decoder_GetDisplayRate( decoder_t *p_dec )
{
    if( !p_dec->pf_get_display_rate )
        return INPUT_RATE_DEFAULT;
    return p_dec->pf_get_display_rate( p_dec );
}
