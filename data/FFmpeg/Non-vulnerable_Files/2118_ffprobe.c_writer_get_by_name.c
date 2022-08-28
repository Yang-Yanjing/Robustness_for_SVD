static const Writer *writer_get_by_name(const char *name)
{
    int i;
    for (i = 0; registered_writers[i]; i++)
        if (!strcmp(registered_writers[i]->name, name))
            return registered_writers[i];
    return NULL;
}
