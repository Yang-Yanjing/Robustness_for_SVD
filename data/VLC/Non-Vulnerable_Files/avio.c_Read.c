}
static ssize_t Read(access_t *access, uint8_t *data, size_t size)
{
    int r = avio_read(access->p_sys->context, data, size);
    if (r > 0)
        access->info.i_pos += r;
    else {
        access->info.b_eof = true;
        r = 0;
    }
    return r;
}
