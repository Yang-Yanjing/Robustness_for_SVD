 */
const char *module_get_capability (const module_t *m)
{
    return (m->psz_capability != NULL) ? m->psz_capability : "none";
}
