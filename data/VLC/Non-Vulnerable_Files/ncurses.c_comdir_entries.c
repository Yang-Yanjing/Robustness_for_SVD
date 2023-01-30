}
static int comdir_entries(const void *a, const void *b)
{
    struct dir_entry_t *dir_entry1 = *(struct dir_entry_t**)a;
    struct dir_entry_t *dir_entry2 = *(struct dir_entry_t**)b;
    if (dir_entry1->file == dir_entry2->file)
        return strcasecmp(dir_entry1->path, dir_entry2->path);
    return dir_entry1->file ? 1 : -1;
}
