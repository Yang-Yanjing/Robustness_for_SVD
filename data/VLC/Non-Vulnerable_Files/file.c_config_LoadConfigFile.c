 *****************************************************************************/
int config_LoadConfigFile( vlc_object_t *p_this )
{
    FILE *file;
    file = config_OpenConfigFile (p_this);
    if (file == NULL)
        return VLC_EGENERIC;
    /* Look for UTF-8 Byte Order Mark */
    char * (*convert) (const char *) = strdupnull;
    char bom[3];
    if ((fread (bom, 1, 3, file) != 3)
     || memcmp (bom, "\xEF\xBB\xBF", 3))
    {
        convert = FromLocaleDup;
        rewind (file); /* no BOM, rewind */
    }
    char *line = NULL;
    size_t bufsize;
    ssize_t linelen;
    /* Ensure consistent number formatting... */
    locale_t loc = newlocale (LC_NUMERIC_MASK, "C", NULL);
    locale_t baseloc = uselocale (loc);
    vlc_rwlock_wrlock (&config_lock);
    while ((linelen = getline (&line, &bufsize, file)) != -1)
    {
        line[linelen - 1] = '\0'; /* trim newline */
        /* Ignore comments, section and empty lines */
        if (memchr ("#[", line[0], 3) != NULL)
            continue;
        /* look for option name */
        const char *psz_option_name = line;
        char *ptr = strchr (line, '=');
        if (ptr == NULL)
            continue; /* syntax error */
        *ptr = '\0';
        module_config_t *item = config_FindConfig (p_this, psz_option_name);
        if (item == NULL)
            continue;
        const char *psz_option_value = ptr + 1;
        switch (CONFIG_CLASS(item->i_type))
        {
            case CONFIG_ITEM_BOOL:
            case CONFIG_ITEM_INTEGER:
            {
                int64_t l;
                errno = 0;
                l = strtoi (psz_option_value);
                if ((l > item->max.i) || (l < item->min.i))
                    errno = ERANGE;
                if (errno)
                    msg_Warn (p_this, "Integer value (%s) for %s: %s",
                              psz_option_value, psz_option_name,
                              vlc_strerror_c(errno));
                else
                    item->value.i = l;
                break;
            }
            case CONFIG_ITEM_FLOAT:
                if (!*psz_option_value)
                    break;                    /* ignore empty option */
                item->value.f = (float)atof (psz_option_value);
                break;
            default:
                free ((char *)item->value.psz);
                item->value.psz = convert (psz_option_value);
                break;
        }
    }
    vlc_rwlock_unlock (&config_lock);
    free (line);
    if (ferror (file))
    {
        msg_Err (p_this, "error reading configuration: %s",
                 vlc_strerror_c(errno));
        clearerr (file);
    }
    fclose (file);
    if (loc != (locale_t)0)
    {
        uselocale (baseloc);
        freelocale (loc);
    }
    return 0;
}
