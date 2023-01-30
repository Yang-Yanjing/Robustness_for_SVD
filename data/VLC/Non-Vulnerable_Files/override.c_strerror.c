 * This caused quite nasty crashes in the history of VLC/Linux. */
char *strerror (int val)
{
    if (override)
    {
        static const char msg[] =
            "Error message unavailable (use strerror_r instead of strerror)!";
        LOG("Blocked", "%d", val);
        return (char *)msg;
    }
    return CALL(strerror, val);
}
