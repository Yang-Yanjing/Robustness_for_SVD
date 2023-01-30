}
static int Seek( access_t* p_access, uint64_t i_pos )
{
    p_access->info.i_pos = i_pos;
    p_access->info.b_eof = false;
    libssh2_sftp_seek( p_access->p_sys->file, i_pos );
    return VLC_SUCCESS;
}
