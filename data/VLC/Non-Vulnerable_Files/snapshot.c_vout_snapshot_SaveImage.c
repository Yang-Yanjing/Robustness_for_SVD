/* */
int vout_snapshot_SaveImage(char **name, int *sequential,
                             const block_t *image,
                             vout_thread_t *p_vout,
                             const vout_snapshot_save_cfg_t *cfg)
{
    /* */
    char *filename;
    DIR *pathdir = vlc_opendir(cfg->path);
    input_thread_t *input = (input_thread_t*)p_vout->p->input;
    if (pathdir != NULL) {
        /* The use specified a directory path */
        closedir(pathdir);
        /* */
        char *prefix = NULL;
        if (cfg->prefix_fmt)
            prefix = str_format(input, cfg->prefix_fmt);
        if (prefix)
            filename_sanitize(prefix);
        else {
            prefix = strdup("vlcsnap-");
            if (!prefix)
                goto error;
        }
        if (cfg->is_sequential) {
            for (int num = cfg->sequence; ; num++) {
                struct stat st;
                if (asprintf(&filename, "%s" DIR_SEP "%s%05d.%s",
                             cfg->path, prefix, num, cfg->format) < 0) {
                    free(prefix);
                    goto error;
                }
                if (vlc_stat(filename, &st)) {
                    *sequential = num;
                    break;
                }
                free(filename);
            }
        } else {
            struct timeval tv;
            struct tm curtime;
            char buffer[128];
            gettimeofday(&tv, NULL);
            if (localtime_r(&tv.tv_sec, &curtime) == NULL)
                gmtime_r(&tv.tv_sec, &curtime);
            if (strftime(buffer, sizeof(buffer), "%Y-%m-%d-%Hh%Mm%Ss",
                         &curtime) == 0)
                strcpy(buffer, "error");
            if (asprintf(&filename, "%s" DIR_SEP "%s%s%03u.%s",
                         cfg->path, prefix, buffer,
                         (unsigned)tv.tv_usec / 1000, cfg->format) < 0)
                filename = NULL;
        }
        free(prefix);
    } else {
        /* The user specified a full path name (including file name) */
        filename = str_format(input, cfg->path);
        path_sanitize(filename);
    }
    if (!filename)
        goto error;
    /* Save the snapshot */
    FILE *file = vlc_fopen(filename, "wb");
    if (!file) {
        msg_Err(p_vout, "Failed to open '%s'", filename);
        free(filename);
        goto error;
    }
    if (fwrite(image->p_buffer, image->i_buffer, 1, file) != 1) {
        msg_Err(p_vout, "Failed to write to '%s'", filename);
        fclose(file);
        free(filename);
        goto error;
    }
    fclose(file);
    /* */
    if (name)
        *name = filename;
    else
        free(filename);
    return VLC_SUCCESS;
error:
    msg_Err(p_vout, "could not save snapshot");
    return VLC_EGENERIC;
}
