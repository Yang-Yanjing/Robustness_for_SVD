}
void bd_mpls_play_item_Parse( bd_mpls_play_item_t *p_item, bs_t *s )
{
    const int i_length = bs_read( s, 16 );
    const int i_start = bs_pos( s ) / 8;
    char psz_name[5+1];
    for( int j = 0; j < 5; j++ )
        psz_name[j] = bs_read( s, 8 );
    psz_name[5] = '\0';
    p_item->clpi.i_id = strtol( psz_name, NULL, 10 );
    bs_skip( s, 32 );
    bs_skip( s, 11 );
    const bool b_angle = bs_read( s, 1 );
    p_item->i_connection = bs_read( s, 4 );
    p_item->clpi.i_stc_id = bs_read( s, 8 );
    p_item->i_in_time = bs_read( s, 32 );
    p_item->i_out_time = bs_read( s, 32 );
    bs_skip( s, 64 );
    bs_skip( s, 1 );
    bs_skip( s, 7 );
    p_item->i_still = bs_read( s, 8 );
    p_item->i_still_time = bs_read( s, 16 );
    if( p_item->i_still == BD_MPLS_PLAY_ITEM_STILL_NONE )
        p_item->i_still_time = 0;
    else if( p_item->i_still == BD_MPLS_PLAY_ITEM_STILL_INFINITE )
        p_item->i_still_time = INT_MAX;
    if( b_angle )
    {
        const int i_angle = bs_read( s, 8 );
        bs_skip( s, 6 );
        p_item->b_angle_different_audio = bs_read( s, 1 );
        p_item->b_angle_seamless = bs_read( s, 1 );
        p_item->p_clpi = calloc( i_angle, sizeof(*p_item->p_clpi) );
        for( p_item->i_clpi = 0; p_item->i_clpi < i_angle; p_item->i_clpi++ )
        {
            if( !p_item->p_clpi )
                break;
            bd_mpls_clpi_t *p_clpi = &p_item->p_clpi[p_item->i_clpi];
            char psz_name[5+1];
            for( int j = 0; j < 5; j++ )
                psz_name[j] = bs_read( s, 8 );
            psz_name[5] = '\0';
            p_clpi->i_id = strtol( psz_name, NULL, 10 );
            bs_skip( s, 32 );
            p_clpi->i_stc_id = bs_read( s, 8 );
        }
    }
    else
    {
        p_item->i_clpi = 0;
        p_item->p_clpi = NULL;
        p_item->b_angle_different_audio = false;
        p_item->b_angle_seamless = true;
    }
    /* STN Table */
    bs_skip( s, 16 );  /* Length */
    bs_skip( s, 16 );
    const int i_video = bs_read( s, 8 );
    const int i_audio = bs_read( s, 8 );
    const int i_pg = bs_read( s, 8 );
    const int i_ig = bs_read( s, 8 );
    const int i_audio_2 = bs_read( s, 8 );
    const int i_video_2 = bs_read( s, 8 );
    const int i_pip_pg = bs_read( s, 8 );
    bs_skip( s, 40 );
    p_item->i_stream = 0;
    p_item->p_stream = calloc( i_video + i_audio + i_pg + i_ig,
                               sizeof(*p_item->p_stream) );
    for( int j = 0; j < i_video; j++, p_item->i_stream++ )
    {
        if( !p_item->p_stream )
            break;
        bd_mpls_stream_Parse( &p_item->p_stream[p_item->i_stream], s,
                              BD_MPLS_STREAM_CLASS_PRIMARY_VIDEO );
    }
    for( int j = 0; j < i_audio; j++, p_item->i_stream++ )
    {
        if( !p_item->p_stream )
            break;
        bd_mpls_stream_Parse( &p_item->p_stream[p_item->i_stream], s,
                              BD_MPLS_STREAM_CLASS_PRIMARY_AUDIO );
    }
    for( int j = 0; j < i_pg; j++, p_item->i_stream++ )
    {
        if( !p_item->p_stream )
            break;
        bd_mpls_stream_Parse( &p_item->p_stream[p_item->i_stream], s,
                              BD_MPLS_STREAM_CLASS_PG );
    }
    for( int j = 0; j < i_ig; j++, p_item->i_stream++ )
    {
        if( !p_item->p_stream )
            break;
        bd_mpls_stream_Parse( &p_item->p_stream[p_item->i_stream], s,
                              BD_MPLS_STREAM_CLASS_IG );
    }
    for( int j = 0; j < i_audio_2; j++ )
    {
        /* TODO I need samples */
    }
    for( int j = 0; j < i_video_2; j++ )
    {
        /* TODO I need samples */
    }
    for( int j = 0; j < i_pip_pg; j++ )
    {
        /* TODO I need samples */
    }
    bs_skip( s, 8 * ( i_start + i_length ) - bs_pos( s ) );
}
