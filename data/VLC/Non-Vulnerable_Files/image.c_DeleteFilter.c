}
static void DeleteFilter( filter_t * p_filter )
{
    if( p_filter->p_module ) module_unneed( p_filter, p_filter->p_module );
    es_format_Clean( &p_filter->fmt_in );
    es_format_Clean( &p_filter->fmt_out );
    vlc_object_release( p_filter );
}
