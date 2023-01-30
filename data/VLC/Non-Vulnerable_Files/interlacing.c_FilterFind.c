}
static char *FilterFind(char *filter_base, const char *module_name)
{
    const size_t module_length = strlen(module_name);
    const char *filter = filter_base;
    if (!filter || module_length <= 0)
        return NULL;
    for (;;) {
        char *start = strstr(filter, module_name);
        if (!start)
            return NULL;
        if (start[module_length] == '\0' || start[module_length] == ':')
            return start;
        filter = &start[module_length];
    }
}
