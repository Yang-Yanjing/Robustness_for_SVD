}
static int alloc_table(VLC *vlc, int size)
{
    int index;
    index = vlc->table_size;
    vlc->table_size += size;
    if (vlc->table_size > vlc->table_allocated) {
        DEBUGF("Tried to allocate past the end of a Huffman table: %d/%d\n", 
            vlc->table_allocated, vlc->table_allocated+(1 << vlc->bits));
        vlc->table_allocated += (1 << vlc->bits);
        if (!vlc->table)
            return -1;
    }
    return index;
}
