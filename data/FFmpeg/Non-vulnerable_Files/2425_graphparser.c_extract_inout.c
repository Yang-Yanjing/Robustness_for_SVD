static AVFilterInOut *extract_inout(const char *label, AVFilterInOut **links)
{
    AVFilterInOut *ret;
    while (*links && (!(*links)->name || strcmp((*links)->name, label)))
        links = &((*links)->next);
    ret = *links;
    if (ret) {
        *links = ret->next;
        ret->next = NULL;
    }
    return ret;
}
