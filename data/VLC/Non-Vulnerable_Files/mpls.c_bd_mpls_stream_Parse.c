/* MPLS */
void bd_mpls_stream_Parse( bd_mpls_stream_t *p_stream, bs_t *s, int i_class )
{
    /* Stream entry parsing */
    const int i_entry_length = bs_read( s, 8 );
    const int i_entry_start = bs_pos( s ) / 8;
    p_stream->i_type = bs_read( s, 8 );
    p_stream->i_class = i_class;
    if( p_stream->i_type == BD_MPLS_STREAM_TYPE_PLAY_ITEM )
    {
        p_stream->play_item.i_pid = bs_read( s, 16 );
    }
    else if( p_stream->i_type == BD_MPLS_STREAM_TYPE_SUB_PATH )
    {
        p_stream->sub_path.i_sub_path_id = bs_read( s, 8 );
        p_stream->sub_path.i_sub_clip_id = bs_read( s, 8 );
        p_stream->sub_path.i_pid = bs_read( s, 16 );
    }
    else if( p_stream->i_type == BD_MPLS_STREAM_TYPE_IN_MUX_SUB_PATH )
    {
        p_stream->in_mux_sub_path.i_sub_path_id = bs_read( s, 8 );
        p_stream->in_mux_sub_path.i_pid = bs_read( s, 16 );
    }
    bs_skip( s, 8 * ( i_entry_start + i_entry_length ) - bs_pos( s ) );
    /* Stream attributes parsing */
    const int i_attributes_length = bs_read( s, 8 );
    const int i_attributes_start = bs_pos( s ) / 8;
    p_stream->i_stream_type = bs_read( s, 8 );
    strcpy( p_stream->psz_language, "" );
    p_stream->i_charset = -1;
    if( p_stream->i_stream_type == 0x02 || /* MPEG-I/II */
        p_stream->i_stream_type == 0x1b || /* AVC */
        p_stream->i_stream_type == 0xea )  /* VC-1 */
    {
        /* Video */
    }
    else if( ( p_stream->i_stream_type >= 0x80 && p_stream->i_stream_type <= 0x8f ) ||
             ( p_stream->i_stream_type >= 0xa0 && p_stream->i_stream_type <= 0xaf ) )
    {
        /* Audio */
        bs_skip( s, 4 );
        bs_skip( s, 4 );
        for( int i = 0; i < 3; i++ )
            p_stream->psz_language[i] = bs_read( s, 8 );
        p_stream->psz_language[3] = '\0';
    }
    else if( p_stream->i_stream_type == 0x90 ||   /* PG stream */
             p_stream->i_stream_type == 0x91 )    /* IG stream */
    {
        for( int i = 0; i < 3; i++ )
            p_stream->psz_language[i] = bs_read( s, 8 );
        p_stream->psz_language[3] = '\0';
    }
    else if( p_stream->i_stream_type == 0x92 )    /* Text stream */
    {
        p_stream->i_charset = bs_read( s, 8 );
        for( int i = 0; i < 3; i++ )
            p_stream->psz_language[i] = bs_read( s, 8 );
        p_stream->psz_language[3] = '\0';
    }
    bs_skip( s, 8 * ( i_attributes_start + i_attributes_length ) - bs_pos( s ) );
}
