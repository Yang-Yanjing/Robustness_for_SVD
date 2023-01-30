 */
static block_t *Block(access_t *access)
{
    imem_sys_t *sys = (imem_sys_t*)access->p_sys;
    unsigned flags;
    size_t buffer_size;
    void   *buffer;
    if (sys->source.get(sys->source.data, sys->source.cookie,
                        NULL, NULL, &flags, &buffer_size, &buffer)) {
        access->info.b_eof = true;
        return NULL;
    }
    block_t *block = NULL;
    if (buffer_size > 0) {
        block = block_Alloc(buffer_size);
        if (block)
            memcpy(block->p_buffer, buffer, buffer_size);
    }
    sys->source.release(sys->source.data, sys->source.cookie,
                        buffer_size, buffer);
    return block;
}
