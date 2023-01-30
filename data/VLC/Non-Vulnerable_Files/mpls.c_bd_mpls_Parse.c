}
int bd_mpls_Parse( bd_mpls_t *p_mpls, bs_t *s, int i_id )
{
    const int i_start = bs_pos( s ) / 8;
    /* */
    if( bs_read( s, 32 ) != 0x4d504c53 )
        return VLC_EGENERIC;
    const uint32_t i_version = bs_read( s, 32 );
    if( i_version != 0x30313030 && i_version != 0x30323030 )
        return VLC_EGENERIC;
    const uint32_t i_play_item_start = bs_read( s, 32 );
    const uint32_t i_mark_start = bs_read( s, 32 );
    bs_skip( s, 32 );   /* Extension start */
    /* */
    p_mpls->i_id = i_id;
    /* Read AppInfo: ignored */
    /* Read Playlist */
    bs_t ps = *s;
    bs_skip( &ps, 8 * ( i_start + i_play_item_start ) - bs_pos( s ) );
    bs_skip( &ps, 32 ); /* Length */
    bs_skip( &ps, 16 );
    const int i_play_item = bs_read( &ps, 16 );
    const int i_sub_path = bs_read( &ps, 16 );
    p_mpls->p_play_item = calloc( i_play_item, sizeof(*p_mpls->p_play_item) );
    for( p_mpls->i_play_item = 0; p_mpls->i_play_item < i_play_item; p_mpls->i_play_item++ )
    {
        if( !p_mpls->p_play_item )
            break;
        bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[p_mpls->i_play_item];
        bd_mpls_play_item_Parse( p_item, &ps );
    }
    p_mpls->p_sub_path = calloc( i_sub_path, sizeof(*p_mpls->p_sub_path) );
    for( p_mpls->i_sub_path = 0; p_mpls->i_sub_path < i_sub_path; p_mpls->i_sub_path++ )
    {
        if( !p_mpls->p_sub_path )
            break;
        bd_mpls_sub_path_t *p_sub = &p_mpls->p_sub_path[p_mpls->i_sub_path];
        bd_mpls_sub_path_Parse( p_sub, &ps );
    }
    /* Read Mark */
    bs_t ms = *s;
    bs_skip( &ms, 8 * ( i_start + i_mark_start ) - bs_pos( s ) );
    bs_skip( &ms, 32 );
    const int i_mark = bs_read( &ms, 16 );
    p_mpls->p_mark = calloc( i_mark, sizeof(*p_mpls->p_mark) );
    for( p_mpls->i_mark = 0; p_mpls->i_mark < i_mark; p_mpls->i_mark++ )
    {
        if( !p_mpls->p_mark )
            break;
        bd_mpls_mark_t *p_mark = &p_mpls->p_mark[p_mpls->i_mark];
        bd_mpls_mark_Parse( p_mark, &ms );
    }
    /* Read Extension: ignored */
    return VLC_SUCCESS;
}
