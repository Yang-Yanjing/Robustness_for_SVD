static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    av_log(NULL, AV_LOG_DEBUG, "\nReceived windows signal %ld\n", fdwCtrlType);
    switch (fdwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        sigterm_handler(SIGINT);
        return TRUE;
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        sigterm_handler(SIGTERM);
        



        while (!ffmpeg_exited) {
            Sleep(0);
        }
        return TRUE;
    default:
        av_log(NULL, AV_LOG_ERROR, "Received unknown windows signal %ld\n", fdwCtrlType);
        return FALSE;
    }
}
