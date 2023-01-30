}
int bd_clpi_Parse( bd_clpi_t *p_clpi, bs_t *s, int i_id )
{
    const int i_start = bs_pos( s ) / 8;
    /* */
    if( bs_read( s, 32 ) != 0x48444D56 )
        return VLC_EGENERIC;
    const uint32_t i_version = bs_read( s, 32 );
    if( i_version != 0x30313030 && i_version != 0x30323030 )
        return VLC_EGENERIC;
    /* */
    const uint32_t i_sequence_start = bs_read( s, 32 );
    const uint32_t i_program_start = bs_read( s, 32 );
    const uint32_t i_cpi_start = bs_read( s, 32 );
    bs_skip( s, 32 );   /* mark start */
    bs_skip( s, 32 );   /* extension start */
    /* */
    p_clpi->i_id = i_id;
    /* Read sequence */
    bs_t ss = *s;
    bs_skip( &ss, 8 * ( i_start + i_sequence_start ) - bs_pos( s ) );
    bs_skip( &ss, 32 ); /* Length */
    bs_skip( &ss, 8 );
    bs_skip( &ss, 8 );  /* ATC sequence count (MUST be 1 ?) */
    bs_skip( &ss, 32 ); /* ATC start (MUST be 0) */
    const int i_stc = bs_read( &ss, 8 );
    bs_skip( &ss, 8 );  /* STC ID offset (MUST be 0 ? */
    p_clpi->p_stc = calloc( i_stc, sizeof(*p_clpi->p_stc) );
    for( p_clpi->i_stc = 0; p_clpi->i_stc < i_stc; p_clpi->i_stc++ )
    {
        if( !p_clpi->p_stc )
            break;
        bd_clpi_stc_Parse( &p_clpi->p_stc[p_clpi->i_stc], &ss );
    }
    /* Program */
    bs_t ps = *s;
    bs_skip( &ps, 8 * ( i_start + i_program_start ) - bs_pos( s ) );
    bs_skip( &ps, 32 ); /* Length */
    bs_skip( &ps, 8 );
    bs_skip( &ps, 8 );  /* Program count (MUST be 1 ?) */
    bs_skip( &ps, 32 ); /* Program sequence start (MUST be 0) */
    p_clpi->i_pmt_pid = bs_read( &ps, 16 );
    const int i_stream = bs_read( &ps, 8 );
    bs_skip( &ps, 8 );  /* Group count (MUST be 1 ?) */
    p_clpi->p_stream = calloc( i_stream, sizeof(*p_clpi->p_stream) );
    for( p_clpi->i_stream = 0; p_clpi->i_stream < i_stream; p_clpi->i_stream++ )
    {
        if( !p_clpi->p_stream )
            break;
        bd_clpi_stream_Parse( &p_clpi->p_stream[p_clpi->i_stream], &ps );
    }
    /* Read CPI */
    bs_t cs = *s;
    bs_skip( &cs, 8 * ( i_start + i_cpi_start ) - bs_pos( s ) );
    const uint32_t i_cpi_length = bs_read( &cs, 32 );
    if( i_cpi_length > 0 )
    {
        bs_skip( &cs, 12 );
        bs_skip( &cs, 4 );  /* Type (MUST be 1) */
        /* EPMap */
        const int i_epmap_start = bs_pos( &cs ) / 8;
        bs_skip( &cs, 8 );
        const int i_ep_map = bs_read( &cs, 8 );
        p_clpi->p_ep_map = calloc( i_ep_map, sizeof(*p_clpi->p_ep_map) );
        for( p_clpi->i_ep_map = 0; p_clpi->i_ep_map < i_ep_map; p_clpi->i_ep_map++ )
        {
            if( !p_clpi->p_ep_map )
                break;
            if( bd_clpi_ep_map_Parse( &p_clpi->p_ep_map[p_clpi->i_ep_map],
                                      &cs, i_epmap_start ) )
                break;
        }
    }
    else
    {
        p_clpi->i_ep_map = 0;
        p_clpi->p_ep_map = NULL;
    }
    return VLC_SUCCESS;
}
