 */
bool module_provides (const module_t *m, const char *cap)
{
    return !strcmp (module_get_capability (m), cap);
}
