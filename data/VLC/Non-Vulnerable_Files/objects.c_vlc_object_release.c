 */
void vlc_object_release( vlc_object_t *p_this )
{
    vlc_object_internals_t *internals = vlc_internals( p_this );
    vlc_object_t *parent = NULL;
    unsigned refs = atomic_load (&internals->refs);
    /* Fast path */
    while (refs > 1)
    {
        if (atomic_compare_exchange_weak (&internals->refs, &refs, refs - 1))
            return; /* There are still other references to the object */
        assert (refs > 0);
    }
    /* Slow path */
    libvlc_lock (p_this->p_libvlc);
    refs = atomic_fetch_sub (&internals->refs, 1);
    assert (refs > 0);
    if (likely(refs == 1))
    {
        /* Detach from parent to protect against vlc_object_find_name() */
        parent = p_this->p_parent;
        if (likely(parent))
        {
           /* Unlink */
           vlc_object_internals_t *prev = internals->prev;
           vlc_object_internals_t *next = internals->next;
           if (prev != NULL)
               prev->next = next;
           else
               vlc_internals (parent)->first = next;
           if (next != NULL)
               next->prev = prev;
        }
        /* We have no children */
        assert (internals->first == NULL);
    }
    libvlc_unlock (p_this->p_libvlc);
    if (likely(refs == 1))
    {
        int canc = vlc_savecancel ();
        vlc_object_destroy( p_this );
        vlc_restorecancel (canc);
        if (parent)
            vlc_object_release (parent);
    }
}
