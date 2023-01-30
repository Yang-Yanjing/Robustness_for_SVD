char *av_append_path_component(const char *path, const char *component)
{
    size_t p_len, c_len;
    char *fullpath;
    if (!path)
        return av_strdup(component);
    if (!component)
        return av_strdup(path);
    p_len = strlen(path);
    c_len = strlen(component);
    if (p_len > SIZE_MAX - c_len || p_len + c_len > SIZE_MAX - 2)
        return NULL;
    fullpath = av_malloc(p_len + c_len + 2);
    if (fullpath) {
        if (p_len) {
            av_strlcpy(fullpath, path, p_len + 1);
            if (c_len) {
                if (fullpath[p_len - 1] != '/' && component[0] != '/')
                    fullpath[p_len++] = '/';
                else if (fullpath[p_len - 1] == '/' && component[0] == '/')
                    p_len--;
            }
        }
        av_strlcpy(&fullpath[p_len], component, c_len + 1);
        fullpath[p_len + c_len] = 0;
    }
    return fullpath;
}
