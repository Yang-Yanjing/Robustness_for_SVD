}
static char *EliminateCRLF(char *psz_string)
{
    char *q;
    for (char * p = psz_string; p && *p; p++) {
        if ((*p == '\r') && (*(p+1) == '\n')) {
            for (q = p + 1; *q; q++)
                *(q - 1) = *q;
            *(q - 1) = '\0';
        }
    }
    return psz_string;
}
