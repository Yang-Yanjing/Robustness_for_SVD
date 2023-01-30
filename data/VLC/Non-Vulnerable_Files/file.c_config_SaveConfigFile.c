 */
int config_SaveConfigFile (vlc_object_t *p_this)
{
    if( config_PrepareDir( p_this ) )
    {
        msg_Err( p_this, "no configuration directory" );
        return -1;
    }
    /*
     * Save module config in file
     */
    char *temporary;
    char *permanent = config_GetConfigFile (p_this);
    if (permanent == NULL)
        return -1;
    if (asprintf (&temporary, "%s.%u", permanent, getpid ()) == -1)
    {
        free (permanent);
        return -1;
    }
    else
    {
        struct stat st;
        /* Some users make vlcrc read-only to prevent changes.
         * The atomic replacement scheme breaks this "feature",
         * so we check for read-only by hand. */
        if (stat (permanent, &st) == 0 && !(st.st_mode & S_IWUSR))
        {
            msg_Err (p_this, "configuration file is read-only");
            goto error;
        }
    }
    /* Configuration lock must be taken before vlcrc serializer below. */
    vlc_rwlock_rdlock (&config_lock);
    /* The temporary configuration file is per-PID. Therefore this function
     * should be serialized against itself within a given process. */
    static vlc_mutex_t lock = VLC_STATIC_MUTEX;
    vlc_mutex_lock (&lock);
    int fd = vlc_open (temporary, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd == -1)
    {
        vlc_rwlock_unlock (&config_lock);
        vlc_mutex_unlock (&lock);
        goto error;
    }
    FILE *file = fdopen (fd, "wt");
    if (file == NULL)
    {
        msg_Err (p_this, "cannot create configuration file: %s",
                 vlc_strerror_c(errno));
        vlc_rwlock_unlock (&config_lock);
        close (fd);
        vlc_mutex_unlock (&lock);
        goto error;
    }
    fprintf( file,
        "\xEF\xBB\xBF###\n"
        "###  "PACKAGE_NAME" "PACKAGE_VERSION"\n"
        "###\n"
        "\n"
        "###\n"
        "### lines beginning with a '#' character are comments\n"
        "###\n"
        "\n" );
    /* Ensure consistent number formatting... */
    locale_t loc = newlocale (LC_NUMERIC_MASK, "C", NULL);
    locale_t baseloc = uselocale (loc);
    /* We would take the config lock here. But this would cause a lock
     * inversion with the serializer above and config_AutoSaveConfigFile().
    vlc_rwlock_rdlock (&config_lock);*/
    /* Look for the selected module, if NULL then save everything */
    size_t count;
    module_t **list = module_list_get (&count);
    for (size_t i = 0; i < count; i++)
    {
        module_t *p_parser = list[i];
        module_config_t *p_item, *p_end;
        if( !p_parser->i_config_items )
            continue;
        fprintf( file, "[%s]", module_get_object (p_parser) );
        if( p_parser->psz_longname )
            fprintf( file, " # %s\n\n", p_parser->psz_longname );
        else
            fprintf( file, "\n\n" );
        for( p_item = p_parser->p_config, p_end = p_item + p_parser->confsize;
             p_item < p_end;
             p_item++ )
        {
            if (!CONFIG_ITEM(p_item->i_type)   /* ignore hint */
             || p_item->b_removed              /* ignore deprecated option */
             || p_item->b_unsaveable)          /* ignore volatile option */
                continue;
            if (IsConfigIntegerType (p_item->i_type))
            {
                int64_t val = p_item->value.i;
                config_Write (file, p_item->psz_text,
                             (CONFIG_CLASS(p_item->i_type) == CONFIG_ITEM_BOOL)
                                  ? N_("boolean") : N_("integer"),
                              val == p_item->orig.i,
                              p_item->psz_name, "%"PRId64, val);
            }
            else
            if (IsConfigFloatType (p_item->i_type))
            {
                float val = p_item->value.f;
                config_Write (file, p_item->psz_text, N_("float"),
                              val == p_item->orig.f,
                              p_item->psz_name, "%f", val);
            }
            else
            {
                const char *psz_value = p_item->value.psz;
                bool modified;
                assert (IsConfigStringType (p_item->i_type));
                modified = !!strcmp (psz_value ? psz_value : "",
                                     p_item->orig.psz ? p_item->orig.psz : "");
                config_Write (file, p_item->psz_text, N_("string"),
                              !modified, p_item->psz_name, "%s",
                              psz_value ? psz_value : "");
            }
        }
    }
    vlc_rwlock_unlock (&config_lock);
    module_list_free (list);
    if (loc != (locale_t)0)
    {
        uselocale (baseloc);
        freelocale (loc);
    }
    /*
     * Flush to disk and replace atomically
     */
    fflush (file); /* Flush from run-time */
    if (ferror (file))
    {
        vlc_unlink (temporary);
        vlc_mutex_unlock (&lock);
        msg_Err (p_this, "cannot write configuration file");
        fclose (file);
        goto error;
    }
#if defined(__APPLE__) || defined(__ANDROID__)
    fsync (fd); /* Flush from OS */
#else
    fdatasync (fd); /* Flush from OS */
#endif
#if defined (_WIN32) || defined (__OS2__)
    /* Windows cannot (re)move open files nor overwrite existing ones */
    fclose (file);
    vlc_unlink (permanent);
#endif
    /* Atomically replace the file... */
    if (vlc_rename (temporary, permanent))
        vlc_unlink (temporary);
    /* (...then synchronize the directory, err, TODO...) */
    /* ...and finally close the file */
    vlc_mutex_unlock (&lock);
#if !defined (_WIN32) && !defined (__OS2__)
    fclose (file);
#endif
    free (temporary);
    free (permanent);
    return 0;
error:
    free (temporary);
    free (permanent);
    return -1;
}
