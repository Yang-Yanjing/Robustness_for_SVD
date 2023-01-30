}
static int read_uint16(ROPacket *p, uint16_t *val)
{
    if (p->pos>p->maxlen-2)
        return 0;
    *val =  (uint16_t)p->data[p->pos  ];
    *val |= (uint16_t)p->data[p->pos+1]<<8;
    p->pos += 2;
    return 1;
}
