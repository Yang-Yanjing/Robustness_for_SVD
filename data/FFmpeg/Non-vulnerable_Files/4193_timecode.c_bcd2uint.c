static unsigned bcd2uint(uint8_t bcd)
{
   unsigned low  = bcd & 0xf;
   unsigned high = bcd >> 4;
   if (low > 9 || high > 9)
       return 0;
   return low + 10*high;
}
