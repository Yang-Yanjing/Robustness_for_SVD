static void put_bitstream_info(AACEncContext *s, const char *name)
{
    int i, namelen, padbits;
    namelen = strlen(name) + 2;
    put_bits(&s->pb, 3, TYPE_FIL);
    put_bits(&s->pb, 4, FFMIN(namelen, 15));
    if (namelen >= 15)
        put_bits(&s->pb, 8, namelen - 14);
    put_bits(&s->pb, 4, 0); 
    padbits = -put_bits_count(&s->pb) & 7;
    avpriv_align_put_bits(&s->pb);
    for (i = 0; i < namelen - 2; i++)
        put_bits(&s->pb, 8, name[i]);
    put_bits(&s->pb, 12 - padbits, 0);
}
