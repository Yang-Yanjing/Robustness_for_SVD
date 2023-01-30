}
static int vlc_sort_key_from_string( const char *psz_name )
{
    static const struct
    {
        const char *psz_name;
        int i_key;
    } pp_keys[] =
        { { "id", SORT_ID },
          { "title", SORT_TITLE },
          { "title nodes first", SORT_TITLE_NODES_FIRST },
          { "artist", SORT_ARTIST },
          { "genre", SORT_GENRE },
          { "random", SORT_RANDOM },
          { "duration", SORT_DURATION },
          { "title numeric", SORT_TITLE_NUMERIC },
          { "album", SORT_ALBUM },
          { NULL, -1 } };
    int i;
    for( i = 0; pp_keys[i].psz_name; i++ )
    {
        if( !strcmp( psz_name, pp_keys[i].psz_name ) )
            return pp_keys[i].i_key;
    }
    return -1;
}
