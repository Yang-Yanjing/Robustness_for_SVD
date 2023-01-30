 *****************************************************************************/
void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    Flush( p_filter );
    free( p_filter->p_sys );
}
