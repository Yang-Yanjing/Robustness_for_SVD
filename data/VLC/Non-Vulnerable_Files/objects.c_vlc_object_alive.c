 */
bool vlc_object_alive(vlc_object_t *obj)
{
    vlc_object_internals_t *internals = vlc_internals (obj);
    return atomic_load (&internals->alive);
}
