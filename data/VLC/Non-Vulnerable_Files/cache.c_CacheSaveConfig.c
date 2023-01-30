        goto error
static int CacheSaveConfig (FILE *file, const module_config_t *cfg)
{
    SAVE_IMMEDIATE (cfg->i_type);
    SAVE_IMMEDIATE (cfg->i_short);
    SAVE_FLAG (cfg->b_advanced);
    SAVE_FLAG (cfg->b_internal);
    SAVE_FLAG (cfg->b_unsaveable);
    SAVE_FLAG (cfg->b_safe);
    SAVE_FLAG (cfg->b_removed);
    SAVE_STRING (cfg->psz_type);
    SAVE_STRING (cfg->psz_name);
    SAVE_STRING (cfg->psz_text);
    SAVE_STRING (cfg->psz_longtext);
    SAVE_IMMEDIATE (cfg->list_count);
    if (IsConfigStringType (cfg->i_type))
    {
        SAVE_STRING (cfg->orig.psz);
        if (cfg->list_count == 0)
            SAVE_IMMEDIATE (cfg->list.psz_cb); /* XXX: see CacheLoadConfig() */
        for (unsigned i = 0; i < cfg->list_count; i++)
            SAVE_STRING (cfg->list.psz[i]);
    }
    else
    {
        SAVE_IMMEDIATE (cfg->orig);
        SAVE_IMMEDIATE (cfg->min);
        SAVE_IMMEDIATE (cfg->max);
        if (cfg->list_count == 0)
            SAVE_IMMEDIATE (cfg->list.i_cb); /* XXX: see CacheLoadConfig() */
        for (unsigned i = 0; i < cfg->list_count; i++)
             SAVE_IMMEDIATE (cfg->list.i[i]);
    }
    for (unsigned i = 0; i < cfg->list_count; i++)
        SAVE_STRING (cfg->list_text[i]);
    return 0;
error:
    return -1;
}
