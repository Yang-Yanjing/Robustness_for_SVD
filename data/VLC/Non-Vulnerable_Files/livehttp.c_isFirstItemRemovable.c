 ************************************************************************/
static bool isFirstItemRemovable( sout_access_out_sys_t *p_sys, uint32_t i_firstseg, uint32_t i_index_offset )
{
    float duration = .0f;
    /* Check that segment has been out of playlist for seglenght + (p_sys->i_numsegs * p_sys->i_seglen) amount
     * We check this by calculating duration of the items that replaced first item in playlist
     */
    for( unsigned int index = 0; index < i_index_offset; index++ )
    {
        output_segment_t *segment = vlc_array_item_at_index( p_sys->segments_t, p_sys->i_segment - i_firstseg + index );
        duration += segment->f_seglength;
    }
    output_segment_t *first = vlc_array_item_at_index( p_sys->segments_t, 0 );
    return duration >= (first->f_seglength + (float)(p_sys->i_numsegs * p_sys->i_seglen));
}
