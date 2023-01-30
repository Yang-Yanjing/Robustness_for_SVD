 */
void CacheSave (vlc_object_t *p_this, const char *dir,
               module_cache_t *entries, size_t n)
{
    char *filename = NULL, *tmpname = NULL;
    if (asprintf (&filename, "%s"DIR_SEP CACHE_NAME, dir ) == -1)
        goto out;
    if (asprintf (&tmpname, "%s.%"PRIu32, filename, (uint32_t)getpid ()) == -1)
        goto out;
    msg_Dbg (p_this, "saving plugins cache %s", filename);
    FILE *file = vlc_fopen (tmpname, "wb");
    if (file == NULL)
    {
        if (errno != EACCES && errno != ENOENT)
            msg_Warn (p_this, "cannot create %s: %s", tmpname,
                      vlc_strerror_c(errno));
        goto out;
    }
    if (CacheSaveBank (file, entries, n))
    {
        msg_Warn (p_this, "cannot write %s: %s", tmpname,
                  vlc_strerror_c(errno));
        clearerr (file);
        fclose (file);
        vlc_unlink (tmpname);
        goto out;
    }
#if !defined( _WIN32 ) && !defined( __OS2__ )
    vlc_rename (tmpname, filename); /* atomically replace old cache */
    fclose (file);
#else
    vlc_unlink (filename);
    fclose (file);
    vlc_rename (tmpname, filename);
#endif
out:
    free (filename);
    free (tmpname);
    for (size_t i = 0; i < n; i++)
        free (entries[i].path);
    free (entries);
}
