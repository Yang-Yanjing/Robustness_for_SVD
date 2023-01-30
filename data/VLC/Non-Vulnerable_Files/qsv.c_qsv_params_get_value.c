}
static uint64_t qsv_params_get_value(const char *const *text,
                                     const int const *list,
                                     size_t size, char *sel)
{
    size_t result = 0;
    if (unlikely(!sel))
        return list[0];
    size /= sizeof(list[0]);
    for (size_t i = 0; i < size; i++)
        if (!strcmp(sel, text[i])) {
            result = i;
            break;
        }
    // sel comes from var_InheritString and must be freed.
    free(sel);
    // Returns the found item, or the default/first one if not found.
    return list[result];
}
