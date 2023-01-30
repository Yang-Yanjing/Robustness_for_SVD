#undef A3
static int HasAncillary( const uint8_t *p_anc )
{
    return ( (p_anc[0] == 0x0 && p_anc[1] == 0xfc && p_anc[2] == 0xff
               && (p_anc[3] & 0x3f) == 0x3f) );
}
