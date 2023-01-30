}
static void OggFillSkeletonFishead( uint8_t *p_buffer, sout_mux_t *p_mux )
{
    memcpy( p_buffer, "fishead", 8 );
    SetWLE( &p_buffer[8], 4 );
    SetWLE( &p_buffer[10], 0 );
    SetQWLE( &p_buffer[20], 1000 );
    SetQWLE( &p_buffer[36], 1000 );
    SetQWLE( &p_buffer[64], p_mux->p_sys->i_pos - p_mux->p_sys->i_segment_start ); /* segment length */
    SetQWLE( &p_buffer[72], p_mux->p_sys->i_data_start - p_mux->p_sys->i_segment_start ); /* data start offset */
}
