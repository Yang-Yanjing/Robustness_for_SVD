}
subpicture_t *filter_chain_SubFilter( filter_chain_t *p_chain, subpicture_t *p_subpic )
{
    for( chained_filter_t *f = p_chain->first; f != NULL; f = f->next )
    {
        filter_t *p_filter = &f->filter;
        p_subpic = p_filter->pf_sub_filter( p_filter, p_subpic );
        if( !p_subpic )
            break;
    }
    return p_subpic;
}
