static const sortfn_t sorting_fns[NUM_SORT_FNS][2];
static inline sortfn_t find_sorting_fn( unsigned i_mode, unsigned i_type )
{
    if( i_mode>=NUM_SORT_FNS || i_type>1 )
        return 0;
    return sorting_fns[i_mode][i_type];
}
