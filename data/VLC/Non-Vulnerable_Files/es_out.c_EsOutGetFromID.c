}
static es_out_id_t *EsOutGetFromID( es_out_t *out, int i_id )
{
    if( i_id < 0 )
    {
        /* Special HACK, -i_id is the cat of the stream */
        return (es_out_id_t*)((uint8_t*)NULL-i_id);
    }
    for( int i = 0; i < out->p_sys->i_es; i++ )
    {
        if( out->p_sys->es[i]->i_id == i_id )
            return out->p_sys->es[i];
    }
    return NULL;
}
