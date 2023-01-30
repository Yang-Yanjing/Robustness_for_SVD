 */
input_thread_t *input_Create( vlc_object_t *p_parent,
                              input_item_t *p_item,
                              const char *psz_log, input_resource_t *p_resource )
{
    return Create( p_parent, p_item, psz_log, false, p_resource );
}
