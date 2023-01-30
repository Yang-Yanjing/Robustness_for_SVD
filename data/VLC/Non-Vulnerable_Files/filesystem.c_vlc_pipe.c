}
int vlc_pipe (int fds[2])
{
#if VLC_WINSTORE_APP
    _set_errno(EPERM);
    return -1;
#else
    return _pipe (fds, 32768, O_NOINHERIT | O_BINARY);
#endif
}
