 */
void * vlc_object_hold( vlc_object_t *p_this )
{
    vlc_object_internals_t *internals = vlc_internals( p_this );
#ifndef NDEBUG
    unsigned refs = atomic_fetch_add (&internals->refs, 1);
    assert (refs > 0); /* Avoid obvious freed object uses */
#else
    atomic_fetch_add (&internals->refs, 1);
#endif
    return p_this;
}
