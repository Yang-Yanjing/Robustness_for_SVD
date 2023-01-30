}
static bool IsSDPString (const char *str)
{
    if (strchr (str, '\r') != NULL)
        return false;
    if (strchr (str, '\n') != NULL)
        return false;
    if (!IsUTF8 (str))
        return false;
    return true;
}
