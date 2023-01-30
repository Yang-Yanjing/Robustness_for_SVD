}
static void demux_IndexClean( demux_index_t *p_idx )
{
    free( p_idx->idx );
    p_idx->idx = NULL;
}
