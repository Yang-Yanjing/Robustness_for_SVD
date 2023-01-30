}
static void *getsym (const char *name)
{
    void *sym = dlsym (RTLD_NEXT, name);
    if (sym == NULL)
    {
        fprintf (stderr, "Cannot resolve symbol %s: %s\n", name,
                 dlerror ());
        abort ();
    }
    return sym;
}
