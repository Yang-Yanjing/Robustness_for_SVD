/* */
static ssize_t Read(access_t *access, uint8_t *buffer, size_t size)
{
    access_sys_t *sys = access->p_sys;
    access->info.b_eof = access->info.i_pos >= (uint64_t)sys->a->i_data;
    if (access->info.b_eof)
        return 0;
    const size_t copy = __MIN(size, sys->a->i_data - access->info.i_pos);
    memcpy(buffer, (uint8_t*)sys->a->p_data + access->info.i_pos, copy);
    access->info.i_pos += copy;
    return copy;
}
