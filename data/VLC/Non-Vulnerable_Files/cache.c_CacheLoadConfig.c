    if (CacheLoadString (&(a), file)) goto error
static int CacheLoadConfig (module_config_t *cfg, FILE *file)
{
    LOAD_IMMEDIATE (cfg->i_type);
    LOAD_IMMEDIATE (cfg->i_short);
    LOAD_FLAG (cfg->b_advanced);
    LOAD_FLAG (cfg->b_internal);
    LOAD_FLAG (cfg->b_unsaveable);
    LOAD_FLAG (cfg->b_safe);
    LOAD_FLAG (cfg->b_removed);
    LOAD_STRING (cfg->psz_type);
    LOAD_STRING (cfg->psz_name);
    LOAD_STRING (cfg->psz_text);
    LOAD_STRING (cfg->psz_longtext);
    LOAD_IMMEDIATE (cfg->list_count);
    if (IsConfigStringType (cfg->i_type))
    {
        LOAD_STRING (cfg->orig.psz);
        if (cfg->orig.psz != NULL)
            cfg->value.psz = strdup (cfg->orig.psz);
        else
            cfg->value.psz = NULL;
        if (cfg->list_count)
            cfg->list.psz = xmalloc (cfg->list_count * sizeof (char *));
        else /* TODO: fix config_GetPszChoices() instead of this hack: */
            LOAD_IMMEDIATE(cfg->list.psz_cb);
        for (unsigned i = 0; i < cfg->list_count; i++)
        {
            LOAD_STRING (cfg->list.psz[i]);
            if (cfg->list.psz[i] == NULL /* NULL -> empty string */
             && (cfg->list.psz[i] = calloc (1, 1)) == NULL)
                goto error;
        }
    }
    else
    {
        LOAD_IMMEDIATE (cfg->orig);
        LOAD_IMMEDIATE (cfg->min);
        LOAD_IMMEDIATE (cfg->max);
        cfg->value = cfg->orig;
        if (cfg->list_count)
            cfg->list.i = xmalloc (cfg->list_count * sizeof (int));
        else /* TODO: fix config_GetPszChoices() instead of this hack: */
            LOAD_IMMEDIATE(cfg->list.i_cb);
        for (unsigned i = 0; i < cfg->list_count; i++)
             LOAD_IMMEDIATE (cfg->list.i[i]);
    }
    cfg->list_text = xmalloc (cfg->list_count * sizeof (char *));
    for (unsigned i = 0; i < cfg->list_count; i++)
    {
        LOAD_STRING (cfg->list_text[i]);
        if (cfg->list_text[i] == NULL /* NULL -> empty string */
         && (cfg->list_text[i] = calloc (1, 1)) == NULL)
            goto error;
    }
    return 0;
error:
    return -1; /* FIXME: leaks */
}
