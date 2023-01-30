 ************************************************************************/
static uint32_t segmentAmountNeeded( sout_access_out_sys_t *p_sys )
{
    float duration = .0f;
    for( unsigned index = 1; (int)index <= vlc_array_count( p_sys->segments_t ); index++ )
    {
        output_segment_t* segment = vlc_array_item_at_index( p_sys->segments_t, vlc_array_count( p_sys->segments_t ) - index );
        duration += segment->f_seglength;
        if( duration >= (float)( 3 * p_sys->i_seglen ) )
            return __MAX(index, p_sys->i_numsegs);
    }
    return vlc_array_count( p_sys->segments_t )-1;
}
