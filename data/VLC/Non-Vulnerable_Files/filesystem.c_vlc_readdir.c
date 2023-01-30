}
char *vlc_readdir (DIR *dir)
{
    vlc_DIR *p_dir = (vlc_DIR *)dir;
    free(p_dir->entry);
#if !VLC_WINSTORE_APP
    /* Drive letters mode */
    if (p_dir->wdir == NULL)
    {
        DWORD drives = p_dir->u.drives;
        if (drives == 0)
        {
            p_dir->entry = NULL;
            return NULL; /* end */
        }
        unsigned int i;
        for (i = 0; !(drives & 1); i++)
            drives >>= 1;
        p_dir->u.drives &= ~(1UL << i);
        assert (i < 26);
        if (asprintf (&p_dir->entry, "%c:\\", 'A' + i) == -1)
            p_dir->entry = NULL;
    }
    else
#endif
    if (p_dir->u.insert_dot_dot)
    {
        /* Adds "..", gruik! */
        p_dir->u.insert_dot_dot = false;
        p_dir->entry = strdup ("..");
    }
    else
    {
        struct _wdirent *ent = _wreaddir (p_dir->wdir);
        p_dir->entry = (ent != NULL) ? FromWide (ent->d_name) : NULL;
    }
    return p_dir->entry;
}
