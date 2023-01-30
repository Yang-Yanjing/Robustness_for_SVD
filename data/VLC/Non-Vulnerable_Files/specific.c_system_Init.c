static HMODULE hWinmm = INVALID_HANDLE_VALUE;
void system_Init(void)
{
    hWinmm = LoadLibrary(TEXT("winmm.dll"));
    if (hWinmm)
    {
        MMRESULT (WINAPI * timeBeginPeriod)(UINT);
        timeBeginPeriod = (void*)GetProcAddress(hWinmm, "timeBeginPeriod");
        if (timeBeginPeriod)
            timeBeginPeriod(5);
    }
    if (system_InitWSA(2, 2) && system_InitWSA(1, 1))
        fputs("Error: cannot initialize Winsocks\n", stderr);
#if !VLC_WINSTORE_APP
# if (_WIN32_WINNT < _WIN32_WINNT_WIN8)
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
                                       "SetDefaultDllDirectories") != NULL)
# endif /* FIXME: not reentrant */
        LoadLibraryFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
#endif
}
