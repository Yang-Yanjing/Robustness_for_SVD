 */
mtime_t decoder_GetDisplayDate( decoder_t *p_dec, mtime_t i_ts )
{
    if( !p_dec->pf_get_display_date )
        return VLC_TS_INVALID;
    return p_dec->pf_get_display_date( p_dec, i_ts );
}
