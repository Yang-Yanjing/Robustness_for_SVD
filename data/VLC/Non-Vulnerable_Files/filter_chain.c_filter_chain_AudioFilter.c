}
block_t *filter_chain_AudioFilter( filter_chain_t *p_chain, block_t *p_block )
{
    for( chained_filter_t *f = p_chain->first; f != NULL; f = f->next )
    {
        filter_t *p_filter = &f->filter;
        p_block = p_filter->pf_audio_filter( p_filter, p_block );
        if( !p_block )
            break;
    }
    return p_block;
}
