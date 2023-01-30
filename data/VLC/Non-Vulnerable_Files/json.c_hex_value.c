typedef unsigned short json_uchar;
static unsigned char hex_value (json_char c)
{
   if (c >= 'A' && c <= 'F')
      return (c - 'A') + 10;
   if (c >= 'a' && c <= 'f')
      return (c - 'a') + 10;
   if (c >= '0' && c <= '9')
      return c - '0';
   return 0xFF;
}
