/* Detect directories that recurse into themselves. */
static bool has_inode_loop (const directory_t *dir, dev_t dev, ino_t inode)
{
    while (dir != NULL)
    {
        if ((dir->device == dev) && (dir->inode == inode))
            return true;
        dir = dir->parent;
    }
    return false;
}
