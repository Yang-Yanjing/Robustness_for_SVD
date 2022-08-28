static void append_inout(AVFilterInOut **inouts, AVFilterInOut **element)
{
    while (*inouts && (*inouts)->next)
        inouts = &((*inouts)->next);
    if (!*inouts)
        *inouts = *element;
    else
        (*inouts)->next = *element;
    *element = NULL;
}
