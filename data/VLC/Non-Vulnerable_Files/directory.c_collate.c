}
static int collate (const char **a, const char **b)
{
#ifdef HAVE_STRCOLL
    return strcoll (*a, *b);
#else
    return strcmp  (*a, *b);
#endif
}
