}
static int actcmp(const void *key, const void *ent)
{
    const struct action *act = ent;
    return strcmp(key, act->name);
}
