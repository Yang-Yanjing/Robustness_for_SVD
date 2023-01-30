}
static void ReadDir(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    if (!sys->current_dir || !*sys->current_dir) {
        msg_Dbg(intf, "no current dir set");
        return;
    }
    DIR *current_dir = vlc_opendir(sys->current_dir);
    if (!current_dir) {
        msg_Warn(intf, "cannot open directory `%s' (%s)", sys->current_dir,
                 vlc_strerror_c(errno));
        return;
    }
    DirsDestroy(sys);
    const char *entry;
    while ((entry = vlc_readdir(current_dir))) {
        if (!sys->show_hidden_files && *entry == '.' && strcmp(entry, ".."))
            continue;
        struct dir_entry_t *dir_entry = malloc(sizeof *dir_entry);
        if (unlikely(dir_entry == NULL))
            continue;
        dir_entry->file = IsFile(sys->current_dir, entry);
        dir_entry->path = xstrdup(entry);
        INSERT_ELEM(sys->dir_entries, sys->n_dir_entries,
             sys->n_dir_entries, dir_entry);
        continue;
    }
    closedir(current_dir);
    if (sys->n_dir_entries > 0)
        qsort(sys->dir_entries, sys->n_dir_entries,
              sizeof(struct dir_entry_t*), &comdir_entries);
}
