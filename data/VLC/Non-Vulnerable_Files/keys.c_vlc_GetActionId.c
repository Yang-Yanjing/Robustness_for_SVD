 */
vlc_action_t vlc_GetActionId (const char *name)
{
    const struct action *act;
    if (strncmp (name, "key-", 4))
        return ACTIONID_NONE;
    name += 4;
    act = bsearch(name, actions, ACTIONS_COUNT, sizeof(*act), actcmp);
    return (act != NULL) ? act->value : ACTIONID_NONE;
}
