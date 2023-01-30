static int match_group_separator(const OptionGroupDef *groups, int nb_groups,
                                 const char *opt)
{
    int i;
    for (i = 0; i < nb_groups; i++) {
        const OptionGroupDef *p = &groups[i];
        if (p->sep && !strcmp(p->sep, opt))
            return i;
    }
    return -1;
}
