/* Parsing */
static char *parse_Attributes(const char *line, const char *attr)
{
    char *p;
    char *begin = (char *) line;
    char *end = begin + strlen(line);
    /* Find start of attributes */
    if ((p = strchr(begin, ':' )) == NULL)
        return NULL;
    begin = p;
    do
    {
        if (strncasecmp(begin, attr, strlen(attr)) == 0
          && begin[strlen(attr)] == '=')
        {
            /* <attr>="<value>"[,]* */
            p = strchr(begin, ',');
            begin += strlen(attr) + 1;
            /* Check if we have " " marked value*/
            if( begin[0] == '"' )
            {
                char *valueend = strchr( begin+1, '"');
                /* No ending " so bail out */
                if( unlikely( !valueend ) )
                    return NULL;
                p = strchr( valueend, ',');
            }
            if (begin >= end)
                return NULL;
            if (p == NULL) /* last attribute */
                return strndup(begin, end - begin);
            /* copy till ',' */
            return strndup(begin, p - begin);
        }
        begin++;
    } while(begin < end);
    return NULL;
}
