 */
module_t *vlc_module_load(vlc_object_t *obj, const char *capability,
                          const char *name, bool strict,
                          vlc_activate_t probe, ...)
{
    char *var = NULL;
    if (name == NULL || name[0] == '\0')
        name = "any";
    /* Deal with variables */
    if (name[0] == '$')
    {
        var = var_InheritString (obj, name + 1);
        name = (var != NULL) ? var : "any";
    }
    /* Find matching modules */
    module_t **mods;
    ssize_t total = module_list_cap (&mods, capability);
    msg_Dbg (obj, "looking for %s module matching \"%s\": %zd candidates",
             capability, name, total);
    if (total <= 0)
    {
        module_list_free (mods);
        msg_Dbg (obj, "no %s modules", capability);
        return NULL;
    }
    module_t *module = NULL;
    const bool b_force_backup = obj->b_force; /* FIXME: remove this */
    va_list args;
    va_start(args, probe);
    while (*name)
    {
        char buf[32];
        size_t slen = strcspn (name, ",");
        if (likely(slen < sizeof (buf)))
        {
            memcpy(buf, name, slen);
            buf[slen] = '\0';
        }
        name += slen;
        name += strspn (name, ",");
        if (unlikely(slen >= sizeof (buf)))
            continue;
        const char *shortcut = buf;
        assert (shortcut != NULL);
        if (!strcasecmp ("none", shortcut))
            goto done;
        obj->b_force = strict && strcasecmp ("any", shortcut);
        for (ssize_t i = 0; i < total; i++)
        {
            module_t *cand = mods[i];
            if (cand == NULL)
                continue; // module failed in previous iteration
            if (!module_match_name (cand, shortcut))
                continue;
            mods[i] = NULL; // only try each module once at most...
            int ret = module_load (obj, cand, probe, args);
            switch (ret)
            {
                case VLC_SUCCESS:
                    module = cand;
                    /* fall through */
                case VLC_ETIMEOUT:
                    goto done;
            }
        }
    }
    /* None of the shortcuts matched, fall back to any module */
    if (!strict)
    {
        obj->b_force = false;
        for (ssize_t i = 0; i < total; i++)
        {
            module_t *cand = mods[i];
            if (cand == NULL || module_get_score (cand) <= 0)
                continue;
            int ret = module_load (obj, cand, probe, args);
            switch (ret)
            {
                case VLC_SUCCESS:
                    module = cand;
                    /* fall through */
                case VLC_ETIMEOUT:
                    goto done;
            }
        }
    }
done:
    va_end (args);
    obj->b_force = b_force_backup;
    module_list_free (mods);
    free (var);
    if (module != NULL)
    {
        msg_Dbg (obj, "using %s module \"%s\"", capability,
                 module_get_object (module));
        vlc_object_set_name (obj, module_get_object (module));
    }
    else
        msg_Dbg (obj, "no %s modules matched", capability);
    return module;
}
