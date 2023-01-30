}
addon_entry_t * addon_entry_Hold( addon_entry_t * p_entry )
{
    addon_entry_owner_t *owner = (addon_entry_owner_t *) p_entry;
    atomic_fetch_add( &owner->refs, 1 );
    return p_entry;
}
