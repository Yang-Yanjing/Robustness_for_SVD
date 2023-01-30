}
int setenv (const char *name, const char *value, int overwrite)
{
    if (override)
    {
        LOG("Blocked", "\"%s\", \"%s\", %d", name, value, overwrite);
        return 0;
    }
    return CALL(setenv, name, value, overwrite);
}
