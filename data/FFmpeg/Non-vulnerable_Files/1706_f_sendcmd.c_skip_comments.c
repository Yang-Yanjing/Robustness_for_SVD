static void skip_comments(const char **buf)
{
    while (**buf) {
        
        *buf += strspn(*buf, SPACES);
        if (**buf != '#')
            break;
        (*buf)++;
        
        *buf += strcspn(*buf, "\n");
        if (**buf)
            (*buf)++;
    }
}
