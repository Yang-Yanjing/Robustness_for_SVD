/* */
static int Seek(access_t *access, uint64_t position)
{
    access->info.i_pos = position;
    access->info.b_eof = false;
    return VLC_SUCCESS;
}
