static inline
void playlist_ItemArraySort( unsigned i_items, playlist_item_t **pp_items,
                             sortfn_t p_sortfn )
{
    if( p_sortfn )
    {
        qsort( pp_items, i_items, sizeof( pp_items[0] ), p_sortfn );
    }
    else /* Randomise */
    {
        unsigned i_position;
        unsigned i_new;
        playlist_item_t *p_temp;
        for( i_position = i_items - 1; i_position > 0; i_position-- )
        {
            i_new = ((unsigned)vlc_mrand48()) % (i_position+1);
            p_temp = pp_items[i_position];
            pp_items[i_position] = pp_items[i_new];
            pp_items[i_new] = p_temp;
        }
    }
}
