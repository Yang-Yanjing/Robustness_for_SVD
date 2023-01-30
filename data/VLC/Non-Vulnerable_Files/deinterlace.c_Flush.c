 *****************************************************************************/
void Flush( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    for( int i = 0; i < METADATA_SIZE; i++ )
    {
        p_sys->meta.pi_date[i] = VLC_TS_INVALID;
        p_sys->meta.pi_nb_fields[i] = 2;
        p_sys->meta.pb_top_field_first[i] = true;
    }
    p_sys->i_frame_offset = 0; /* reset to default value (first frame after
                                  flush cannot have offset) */
    for( int i = 0; i < HISTORY_SIZE; i++ )
    {
        if( p_sys->pp_history[i] )
            picture_Release( p_sys->pp_history[i] );
        p_sys->pp_history[i] = NULL;
    }
    IVTCClearState( p_filter );
}
