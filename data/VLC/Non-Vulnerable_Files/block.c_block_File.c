 */
block_t *block_File (int fd)
{
    size_t length;
    struct stat st;
    /* First, get the file size */
    if (fstat (fd, &st))
        return NULL;
    /* st_size is meaningful for regular files, shared memory and typed memory.
     * It's also meaning for symlinks, but that's not possible with fstat().
     * In other cases, it's undefined, and we should really not go further. */
#ifndef S_TYPEISSHM
# define S_TYPEISSHM( buf ) (0)
#endif
    if (S_ISDIR (st.st_mode))
    {
        errno = EISDIR;
        return NULL;
    }
    if (!S_ISREG (st.st_mode) && !S_TYPEISSHM (&st))
    {
        errno = ESPIPE;
        return NULL;
    }
    /* Prevent an integer overflow in mmap() and malloc() */
    if ((uintmax_t)st.st_size >= SIZE_MAX)
    {
        errno = ENOMEM;
        return NULL;
    }
    length = (size_t)st.st_size;
#ifdef HAVE_MMAP
    if (length > 0)
    {
        void *addr;
        addr = mmap (NULL, length, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (addr != MAP_FAILED)
            return block_mmap_Alloc (addr, length);
    }
#endif
    /* If mmap() is not implemented by the OS _or_ the filesystem... */
    block_t *block = block_Alloc (length);
    if (block == NULL)
        return NULL;
    block_cleanup_push (block);
    for (size_t i = 0; i < length;)
    {
        ssize_t len = pread (fd, block->p_buffer + i, length - i, i);
        if (len == -1)
        {
            block_Release (block);
            block = NULL;
            break;
        }
        i += len;
    }
    vlc_cleanup_pop ();
    return block;
}
