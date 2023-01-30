 */
const char *module_gettext (const module_t *m, const char *str)
{
    if (m->parent != NULL)
        m = m->parent;
    if (unlikely(str == NULL || *str == '\0'))
        return "";
#ifdef ENABLE_NLS
    const char *domain = m->domain;
    return dgettext ((domain != NULL) ? domain : PACKAGE_NAME, str);
#else
    (void)m;
    return str;
#endif
}
