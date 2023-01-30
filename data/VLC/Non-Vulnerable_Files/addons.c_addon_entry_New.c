 *****************************************************************************/
addon_entry_t * addon_entry_New()
{
    addon_entry_owner_t *owner = calloc( 1, sizeof(addon_entry_owner_t) );
    if( unlikely(owner == NULL) )
        return NULL;
    atomic_init( &owner->refs, 1 );
    addon_entry_t *p_entry = &owner->entry;
    vlc_mutex_init( &p_entry->lock );
    ARRAY_INIT( p_entry->files );
    return p_entry;
}
