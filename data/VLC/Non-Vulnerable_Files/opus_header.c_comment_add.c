}
static int comment_add(char **comments, size_t *length, const char *tag,
                       const char *val)
{
    char *p = *comments;
    int vendor_length = GetDWLE(p + 8);
    size_t user_comment_list_length = GetDWLE(p + 8 + 4 + vendor_length);
    size_t tag_len = (tag ? strlen(tag) : 0);
    size_t val_len = strlen(val);
    size_t len = (*length) + 4 + tag_len + val_len;
    p = realloc(p, len);
    if (p == NULL)
        return 1;
    SetDWLE(p + *length, tag_len + val_len);          /* length of comment */
    if (tag) memcpy(p + *length + 4, tag, tag_len);         /* comment */
    memcpy(p + *length + 4 + tag_len, val, val_len);        /* comment */
    SetDWLE(p + 8 + 4 + vendor_length, user_comment_list_length + 1);
    *comments = p;
    *length = len;
    return 0;
}
