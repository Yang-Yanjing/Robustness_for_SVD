#   define PADDING_SPACES 25
static void print_section(const module_t *m, const module_config_t **sect,
                          bool color, bool desc)
{
    const module_config_t *item = *sect;
    if (item == NULL)
        return;
    *sect = NULL;
    printf(color ? RED"   %s:\n"GRAY : "   %s:\n",
           module_gettext(m, item->psz_text));
    if (desc && item->psz_longtext != NULL)
        printf(color ? MAGENTA"   %s\n"GRAY : "   %s\n",
               module_gettext(m, item->psz_longtext));
}
