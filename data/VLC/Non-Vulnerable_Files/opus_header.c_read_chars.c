}
static int read_chars(ROPacket *p, unsigned char *str, int nb_chars)
{
    if (p->pos>p->maxlen-nb_chars)
        return 0;
    for (int i=0;i<nb_chars;i++)
        str[i] = p->data[p->pos++];
    return 1;
}
