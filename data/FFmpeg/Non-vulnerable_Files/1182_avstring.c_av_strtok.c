char *av_strtok(char *s, const char *delim, char **saveptr)
{
    char *tok;
    if (!s && !(s = *saveptr))
        return NULL;
    
    s += strspn(s, delim);
    
    if (!*s) {
        *saveptr = NULL;
        return NULL;
    }
    tok = s++;
    
    s += strcspn(s, delim);
    if (*s) {
        *s = 0;
        *saveptr = s+1;
    } else {
        *saveptr = NULL;
    }
    return tok;
}
