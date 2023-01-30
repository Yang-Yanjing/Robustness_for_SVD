}
int unsetenv (const char *name)
{
    return setenv (name, "", 1);
}
