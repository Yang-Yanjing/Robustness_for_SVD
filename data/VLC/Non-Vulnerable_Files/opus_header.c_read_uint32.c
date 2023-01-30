}
static int read_uint32(ROPacket *p, uint32_t *val)
{
    if (p->pos>p->maxlen-4)
        return 0;
    *val =  (uint32_t)p->data[p->pos  ];
    *val |= (uint32_t)p->data[p->pos+1]<< 8;
    *val |= (uint32_t)p->data[p->pos+2]<<16;
    *val |= (uint32_t)p->data[p->pos+3]<<24;
    p->pos += 4;
    return 1;
}
