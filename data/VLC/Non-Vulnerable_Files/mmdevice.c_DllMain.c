BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID); /* avoid warning */
BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
    (void) dll;
    (void) reserved;
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        {
            HANDLE h = GetModuleHandle(TEXT("kernel32.dll"));
            if (unlikely(h == NULL))
                return FALSE;
            LOOKUP(InitializeConditionVariable);
            LOOKUP(SleepConditionVariableCS);
            LOOKUP(WakeConditionVariable);
            break;
        }
    }
    return TRUE;
}
