 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_Destroy(p_filter->p_libvlc, "audiobargraph_v-i_values");
    var_Destroy(p_filter->p_libvlc, "audiobargraph_v-alarm");
    while (p_sys->first != NULL) {
        ValueDate_t *current = p_sys->first;
        p_sys->first = p_sys->first->next;
        free(current);
    }
    free(p_sys);
}
