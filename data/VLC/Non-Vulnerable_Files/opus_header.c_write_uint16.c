}
static int write_uint16(Packet *p, uint16_t val)
{
    if (p->pos>p->maxlen-2)
        return 0;
    p->data[p->pos  ] = (val    ) & 0xFF;
    p->data[p->pos+1] = (val>> 8) & 0xFF;
    p->pos += 2;
    return 1;
}
