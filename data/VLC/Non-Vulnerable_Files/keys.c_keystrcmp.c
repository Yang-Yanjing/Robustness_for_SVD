#define KEYS_COUNT (sizeof(vlc_keys)/sizeof(vlc_keys[0]))
static int keystrcmp (const void *key, const void *elem)
{
    const char *sa = key, *sb = elem;
    return strcmp (sa, sb);
}
