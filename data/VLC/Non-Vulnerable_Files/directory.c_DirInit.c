}
int DirInit (access_t *p_access, DIR *handle)
{
    access_sys_t *p_sys = malloc (sizeof (*p_sys));
    if (unlikely(p_sys == NULL))
        goto error;
    char *uri;
    if (!strcmp (p_access->psz_access, "fd"))
    {
        if (asprintf (&uri, "fd://%s", p_access->psz_location) == -1)
            uri = NULL;
    }
    else
        uri = vlc_path2uri (p_access->psz_filepath, "file");
    if (unlikely(uri == NULL))
        goto error;
    /* "Open" the base directory */
    directory_t *root = malloc (sizeof (*root));
    if (unlikely(root == NULL))
    {
        free (uri);
        goto error;
    }
    char *psz_sort = var_InheritString (p_access, "directory-sort");
    if (!psz_sort)
        p_sys->compar = collate;
    else if (!strcasecmp (psz_sort, "version"))
        p_sys->compar = version;
    else if (!strcasecmp (psz_sort, "none"))
        p_sys->compar = NULL;
    else
        p_sys->compar = collate;
    free(psz_sort);
    root->parent = NULL;
    root->handle = handle;
    root->uri = uri;
    root->filec = vlc_loaddir (handle, &root->filev, visible, p_sys->compar);
    if (root->filec < 0)
        root->filev = NULL;
    root->i = 0;
#ifdef HAVE_OPENAT
    struct stat st;
    if (fstat (dirfd (handle), &st))
    {
        free (root);
        free (uri);
        goto error;
    }
    root->device = st.st_dev;
    root->inode = st.st_ino;
#else
    root->path = strdup (p_access->psz_filepath);
#endif
    p_access->p_sys = p_sys;
    p_sys->current = root;
    p_sys->ignored_exts = var_InheritString (p_access, "ignore-filetypes");
    p_sys->header = true;
    p_sys->i_item_count = 0;
    p_sys->xspf_ext = strdup ("");
    /* Handle mode */
    char *psz = var_InheritString (p_access, "recursive");
    if (psz == NULL || !strcasecmp (psz, "none"))
        p_sys->mode = MODE_NONE;
    else if( !strcasecmp( psz, "collapse" )  )
        p_sys->mode = MODE_COLLAPSE;
    else
        p_sys->mode = MODE_EXPAND;
    free( psz );
    access_InitFields(p_access);
    p_access->pf_read  = NULL;
    p_access->pf_block = DirBlock;
    p_access->pf_seek  = NULL;
    p_access->pf_control = DirControl;
    return VLC_SUCCESS;
error:
    closedir (handle);
    free (p_sys);
    return VLC_EGENERIC;
}
