void av_file_unmap(uint8_t *bufptr, size_t size)
{
#if HAVE_MMAP
    munmap(bufptr, size);
#elif HAVE_MAPVIEWOFFILE
    UnmapViewOfFile(bufptr);
#else
    av_free(bufptr);
#endif
}
