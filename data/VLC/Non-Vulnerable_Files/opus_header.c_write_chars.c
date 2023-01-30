}
static int write_chars(Packet *p, const unsigned char *str, int nb_chars)
{
    if (p->pos>p->maxlen-nb_chars)
        return 0;
    for (int i=0;i<nb_chars;i++)
        p->data[p->pos++] = str[i];
    return 1;
}
