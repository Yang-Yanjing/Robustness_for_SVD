/*** DVB-T ***/
static uint32_t dvb_parse_bandwidth (uint32_t i)
{
    switch (i)
    {
      //case  0: return 0;
        case  2: return 1712000;
        default: return i * 1000000;
    }
}
