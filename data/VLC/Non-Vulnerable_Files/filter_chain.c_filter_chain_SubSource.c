}
void filter_chain_SubSource( filter_chain_t *p_chain,
                             mtime_t display_date )
{
    for( chained_filter_t *f = p_chain->first; f != NULL; f = f->next )
    {
        filter_t *p_filter = &f->filter;
        subpicture_t *p_subpic = p_filter->pf_sub_source( p_filter, display_date );
        /* XXX I find that spu_t cast ugly */
        if( p_subpic )
            spu_PutSubpicture( (spu_t*)p_chain->p_this, p_subpic );
    }
}
