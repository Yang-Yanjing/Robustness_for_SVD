}
static void print_item(const module_t *m, const module_config_t *item,
                       const module_config_t **section, bool color, bool desc)
{
#ifndef _WIN32
# define OPTION_VALUE_SEP " "
#else
# define OPTION_VALUE_SEP "="
#endif
    const char *bra = OPTION_VALUE_SEP "<", *type, *ket = ">";
    const char *prefix = NULL, *suffix = "";
    char psz_buffer[10000]; // XXX
    switch (CONFIG_CLASS(item->i_type))
    {
        case 0: // hint class
            switch (item->i_type)
            {
                case CONFIG_HINT_CATEGORY:
                case CONFIG_HINT_USAGE:
                    printf(color ? GREEN "\n %s\n" GRAY : "\n %s\n",
                           module_gettext(m, item->psz_text));
                    if (desc && item->psz_longtext != NULL)
                        printf(color ? CYAN " %s\n" GRAY : " %s\n",
                               module_gettext(m, item->psz_longtext));
                    break;
                case CONFIG_SECTION:
                    *section = item;
                    break;
            }
            return;
        case CONFIG_ITEM_STRING:
            type = _("string");
            if (item->list_count > 0)
            {
                bra = OPTION_VALUE_SEP "{";
                type = psz_buffer;
                psz_buffer[0] = '\0';
                for (unsigned i = 0; i < item->list_count; i++)
                {
                    if (i > 0)
                        strcat(psz_buffer, ",");
                    strcat(psz_buffer, item->list.psz[i]);
                }
                ket = "}";
            }
            break;
        case CONFIG_ITEM_INTEGER:
            type = _("integer");
            if (item->min.i != 0 || item->max.i != 0)
            {
                sprintf (psz_buffer, "%s [%"PRId64" .. %"PRId64"]",
                         type, item->min.i, item->max.i);
                type = psz_buffer;
            }
            if (item->list_count > 0)
            {
                bra = OPTION_VALUE_SEP "{";
                type = psz_buffer;
                psz_buffer[0] = '\0';
                for (unsigned i = 0; i < item->list_count; i++)
                {
                    if (i != 0)
                        strcat(psz_buffer, ", ");
                    sprintf(psz_buffer + strlen(psz_buffer), "%i (%s)",
                            item->list.i[i],
                            module_gettext(m, item->list_text[i]));
                }
                ket = "}";
            }
            break;
        case CONFIG_ITEM_FLOAT:
            type = _("float");
            if (item->min.f != 0.f || item->max.f != 0.f)
            {
                sprintf(psz_buffer, "%s [%f .. %f]", type,
                        item->min.f, item->max.f);
                type = psz_buffer;
            }
            break;
        case CONFIG_ITEM_BOOL:
            bra = type = ket = "";
            prefix = ", --no-";
            suffix = item->value.i ? _(" (default enabled)")
                                   : _(" (default disabled)");
            break;
       default:
            return;
    }
    print_section(m, section, color, desc);
    /* Add short option if any */
    char shortopt[4];
    if (item->i_short != '\0')
        sprintf(shortopt, "-%c,", item->i_short);
    else
        strcpy(shortopt, "   ");
    if (CONFIG_CLASS(item->i_type) == CONFIG_ITEM_BOOL)
        printf(color ? WHITE"  %s --%s"      "%s%s%s%s%s "GRAY
                     : "  %s --%s%s%s%s%s%s ", shortopt, item->psz_name,
               prefix, item->psz_name, bra, type, ket);
    else
        printf(color ? WHITE"  %s --%s"YELLOW"%s%s%s%s%s "GRAY
                     : "  %s --%s%s%s%s%s%s ", shortopt, item->psz_name,
               "", "",  /* XXX */      bra, type, ket);
    /* Wrap description */
    int offset = PADDING_SPACES - strlen(item->psz_name)
               - strlen(bra) - vlc_swidth(type) - strlen(ket) - 1;
    if (CONFIG_CLASS(item->i_type) == CONFIG_ITEM_BOOL)
        offset -= strlen(item->psz_name) + vlc_swidth(prefix);
    if (offset < 0)
    {
        putchar('\n');
        offset = PADDING_SPACES + LINE_START;
    }
    printf("%*s", offset, "");
    sprintf(psz_buffer, "%s%s", module_gettext(m, item->psz_text), suffix);
    print_desc(psz_buffer, PADDING_SPACES + LINE_START, color);
    if (desc && (item->psz_longtext != NULL && item->psz_longtext[0]))
    {   /* Wrap long description */
        printf("%*s", LINE_START + 2, "");
        sprintf(psz_buffer, "%s%s", module_gettext(m, item->psz_longtext),
                suffix);
        print_desc(psz_buffer, LINE_START + 2, false);
    }
}
