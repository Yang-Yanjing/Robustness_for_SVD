static void dump_argument(const char *a)
{
    const unsigned char *p;
    for (p = a; *p; p++)
        if (!((*p >= '+' && *p <= ':') || (*p >= '@' && *p <= 'Z') ||
              *p == '_' || (*p >= 'a' && *p <= 'z')))
            break;
    if (!*p) {
        fputs(a, report_file);
        return;
    }
    fputc('"', report_file);
    for (p = a; *p; p++) {
        if (*p == '\\' || *p == '"' || *p == '$' || *p == '`')
            fprintf(report_file, "\\%c", *p);
        else if (*p < ' ' || *p > '~')
            fprintf(report_file, "\\x%02x", *p);
        else
            fputc(*p, report_file);
    }
    fputc('"', report_file);
}
