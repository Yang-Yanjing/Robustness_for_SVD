}
static int next_track( stream_t *s )
{
    stream_sys_t *p_sys = s->p_sys;
    uint64_t tmp, min = 0;
    int cat, ret = UNKNOWN_ES;
    for( int i = 0; i < 3; i++ )
    {
        tmp = p_sys->download.lead[i];
        cat = index_to_es_cat( i );
        if( (!min || tmp < min) && SMS_GET_SELECTED_ST( cat ) )
        {
            min = tmp;
            ret = cat;
        }
    }
    return ret;
}
