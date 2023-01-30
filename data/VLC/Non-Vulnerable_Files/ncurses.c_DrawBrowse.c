}
static int DrawBrowse(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    for (int i = 0; i < sys->n_dir_entries; i++) {
        struct dir_entry_t *dir_entry = sys->dir_entries[i];
        char type = dir_entry->file ? ' ' : '+';
        if (sys->color)
            color_set(dir_entry->file ? C_DEFAULT : C_FOLDER, NULL);
        MainBoxWrite(sys, i, " %c %s", type, dir_entry->path);
    }
    return sys->n_dir_entries;
}
