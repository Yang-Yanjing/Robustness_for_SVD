 */
block_t *block_FilePath (const char *path)
{
    int fd = vlc_open (path, O_RDONLY);
    if (fd == -1)
        return NULL;
    block_t *block = block_File (fd);
    close (fd);
    return block;
}
