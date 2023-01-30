#define RI_MMI                         0x00400041
static int ResourceIdToInt( uint8_t *p_data )
{
    return ((int)p_data[0] << 24) | ((int)p_data[1] << 16)
            | ((int)p_data[2] << 8) | p_data[3];
}
