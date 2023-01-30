 *****************************************************************************/
static void DirsDestroy(intf_sys_t *sys)
{
    while (sys->n_dir_entries) {
        struct dir_entry_t *dir_entry = sys->dir_entries[--sys->n_dir_entries];
        free(dir_entry->path);
        free(dir_entry);
    }
    free(sys->dir_entries);
    sys->dir_entries = NULL;
}
