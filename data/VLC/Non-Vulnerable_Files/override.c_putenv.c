 * they could crash the process. This may cause funny behaviour though. */
int putenv (char *str)
{
    if (override)
    {
        LOG("Blocked", "\"%s\"", str);
        return 0;
    }
    return CALL(putenv, str);
}
