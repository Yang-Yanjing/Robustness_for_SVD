 */
static void MaskToShift( int *pi_left, int *pi_right, uint32_t i_mask )
{
    uint32_t i_low, i_high;            /* lower and higher bits of the mask */
    if( !i_mask )
    {
        *pi_left = *pi_right = 0;
        return;
    }
    /* Get bits */
    i_low = i_high = i_mask;
    i_low &= - (int32_t)i_low;          /* lower bit of the mask */
    i_high += i_low;                    /* higher bit of the mask */
    /* Transform bits into an index. Also deal with i_high overflow, which
     * is faster than changing the BinaryLog code to handle 64 bit integers. */
    i_low =  BinaryLog (i_low);
    i_high = i_high ? BinaryLog (i_high) : 32;
    /* Update pointers and return */
    *pi_left =   i_low;
    *pi_right = (8 - i_high + i_low);
}
