}
static bool IsFile(const char *current_dir, const char *entry)
{
    bool ret = true;
#ifdef S_ISDIR
    char *uri;
    if (asprintf(&uri, "%s" DIR_SEP "%s", current_dir, entry) != -1) {
        struct stat st;
        ret = vlc_stat(uri, &st) || !S_ISDIR(st.st_mode);
        free(uri);
    }
#endif
    return ret;
}
