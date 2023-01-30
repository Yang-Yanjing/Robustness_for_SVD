}
static inline int bs_read_se( bs_t *s )
{
    int val = bs_read_ue( s );
    return val&0x01 ? (val+1)/2 : -(val/2);
}
