 */
void system_End(void)
{
    if (hWinmm)
    {
        MMRESULT (WINAPI * timeEndPeriod)(UINT);
        timeEndPeriod = (void*)GetProcAddress(hWinmm, "timeEndPeriod");
        if (timeEndPeriod)
            timeEndPeriod(5);
        FreeLibrary(hWinmm);
    }
    /* XXX: In theory, we should not call this if WSAStartup() failed. */
    WSACleanup();
}
