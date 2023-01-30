}
void ff_put_string(PutBitContext * pbc, char *s, int put_zero)
{
    while(*s){
        put_bits(pbc, 8, *s);
        s++;
    }
    if(put_zero)
        put_bits(pbc, 8, 0);
}
