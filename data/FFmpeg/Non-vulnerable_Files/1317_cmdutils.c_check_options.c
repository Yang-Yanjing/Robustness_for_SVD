static void check_options(const OptionDef *po)
{
    while (po->name) {
        if (po->flags & OPT_PERFILE)
            av_assert0(po->flags & (OPT_INPUT | OPT_OUTPUT));
        po++;
    }
}
