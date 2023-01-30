 * when called with an empty argument or just '\'. */
DIR *vlc_opendir (const char *dirname)
{
    wchar_t *wpath = widen_path (dirname);
    if (wpath == NULL)
        return NULL;
    vlc_DIR *p_dir = malloc (sizeof (*p_dir));
    if (unlikely(p_dir == NULL))
    {
        free(wpath);
        return NULL;
    }
#if !VLC_WINSTORE_APP
    /* Special mode to list drive letters */
    if (wpath[0] == L'\0' || (wcscmp (wpath, L"\\") == 0))
    {
        free (wpath);
        p_dir->wdir = NULL;
        p_dir->u.drives = GetLogicalDrives ();
        p_dir->entry = NULL;
        return (void *)p_dir;
    }
#endif
    assert (wpath[0]); // wpath[1] is defined
    p_dir->u.insert_dot_dot = !wcscmp (wpath + 1, L":\\");
    _WDIR *wdir = _wopendir (wpath);
    free (wpath);
    if (wdir == NULL)
    {
        free (p_dir);
        return NULL;
    }
    p_dir->wdir = wdir;
    p_dir->entry = NULL;
    return (void *)p_dir;
}
