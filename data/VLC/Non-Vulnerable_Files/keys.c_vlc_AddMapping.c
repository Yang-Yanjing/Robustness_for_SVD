 */
static int vlc_AddMapping (void **map, uint32_t keycode, vlc_action_t action)
{
    struct mapping *entry = malloc (sizeof (*entry));
    if (entry == NULL)
        return ENOMEM;
    entry->key = keycode;
    entry->action = action;
    struct mapping **pent = tsearch (entry, map, keycmp);
    if (unlikely(pent == NULL))
        return ENOMEM;
    if (*pent != entry)
    {
        free (entry);
        return EEXIST;
    }
    return 0;
}
