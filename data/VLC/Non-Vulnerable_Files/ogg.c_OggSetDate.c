}
static void OggSetDate( block_t *p_og, mtime_t i_dts, mtime_t i_length )
{
    int i_count;
    block_t *p_tmp;
    mtime_t i_delta;
    for( p_tmp = p_og, i_count = 0; p_tmp != NULL; p_tmp = p_tmp->p_next )
    {
        i_count++;
    }
    if( i_count == 0 ) return; /* ignore. */
    i_delta = i_length / i_count;
    for( p_tmp = p_og; p_tmp != NULL; p_tmp = p_tmp->p_next )
    {
        p_tmp->i_dts    = i_dts;
        p_tmp->i_length = i_delta;
        i_dts += i_delta;
    }
}
