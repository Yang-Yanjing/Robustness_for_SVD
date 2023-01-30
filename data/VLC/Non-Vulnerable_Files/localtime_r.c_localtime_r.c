 * thread-specific storage. */
struct tm *localtime_r (const time_t *timep, struct tm *result)
{
    struct tm *s = localtime (timep);
    if (s == NULL)
        return NULL;
    *result = *s;
    return result;
}
