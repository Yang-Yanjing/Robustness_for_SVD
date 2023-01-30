 */
static int EnumDevices(vlc_object_t *obj, char const *varname,
                       char ***restrict idp, char ***restrict namep)
{
    void **hints;
    msg_Dbg (obj, "Available ALSA PCM devices:");
    if (snd_device_name_hint(-1, "pcm", &hints) < 0)
        return -1;
    char **ids = NULL, **names = NULL;
    unsigned n = 0;
    bool hinted_default = false;
    for (size_t i = 0; hints[i] != NULL; i++)
    {
        void *hint = hints[i];
        char *name = snd_device_name_get_hint(hint, "NAME");
        if (unlikely(name == NULL))
            continue;
        char *desc = snd_device_name_get_hint(hint, "DESC");
        if (desc == NULL)
            desc = xstrdup (name);
        for (char *lf = strchr(desc, '\n'); lf; lf = strchr(lf, '\n'))
            *lf = ' ';
        msg_Dbg (obj, "%s (%s)", (desc != NULL) ? desc : name, name);
        ids = xrealloc (ids, (n + 1) * sizeof (*ids));
        names = xrealloc (names, (n + 1) * sizeof (*names));
        ids[n] = name;
        names[n] = desc;
        n++;
        if (!strcmp(name, "default"))
            hinted_default = true;
    }
    snd_device_name_free_hint(hints);
    if (!hinted_default)
    {
        ids = xrealloc (ids, (n + 1) * sizeof (*ids));
        names = xrealloc (names, (n + 1) * sizeof (*names));
        ids[n] = xstrdup ("default");
        names[n] = xstrdup (_("Default"));
        n++;
    }
    *idp = ids;
    *namep = names;
    (void) varname;
    return n;
}
