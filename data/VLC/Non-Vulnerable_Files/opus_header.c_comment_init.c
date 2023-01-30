*/
static char *comment_init(size_t *length, const char *vendor)
{
    /*The 'vendor' field should be the actual encoding library used.*/
    if (!vendor)
        vendor = "unknown";
    int vendor_length = strlen(vendor);
    int user_comment_list_length = 0;
    int len = 8 + 4 + vendor_length + 4;
    char *p = malloc(len);
    if (p == NULL)
        return NULL;
    memcpy(p, "OpusTags", 8);
    SetDWLE(p + 8, vendor_length);
    memcpy(p + 12, vendor, vendor_length);
    SetDWLE(p + 12 + vendor_length, user_comment_list_length);
    *length = len;
    return p;
}
