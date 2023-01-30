}
FILE *vlc_win32_tmpfile(void)
{
    TCHAR tmp_path[MAX_PATH-14];
    int i_ret = GetTempPath (MAX_PATH-14, tmp_path);
    if (i_ret == 0)
        return NULL;
    TCHAR tmp_name[MAX_PATH];
    i_ret = GetTempFileName(tmp_path, TEXT("VLC"), 0, tmp_name);
    if (i_ret == 0)
        return NULL;
    HANDLE hFile = CreateFile(tmp_name,
            GENERIC_READ | GENERIC_WRITE | DELETE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return NULL;
    int fd = _open_osfhandle((intptr_t)hFile, 0);
    if (fd == -1) {
        CloseHandle(hFile);
        return NULL;
    }
    FILE *stream = _fdopen(fd, "w+b");
    if (stream == NULL) {
        _close(fd);
        return NULL;
    }
    return stream;
}
