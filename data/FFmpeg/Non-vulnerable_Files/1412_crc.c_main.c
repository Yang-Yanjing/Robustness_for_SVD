int main(void)
{
    uint8_t buf[1999];
    int i;
    unsigned
        p[6][3] = { { AV_CRC_32_IEEE_LE, 0xEDB88320, 0x3D5CDD04 },
                    { AV_CRC_32_IEEE   , 0x04C11DB7, 0xC0F5BAE0 },
                    { AV_CRC_24_IEEE   , 0x864CFB  , 0xB704CE   },
                    { AV_CRC_16_ANSI_LE, 0xA001    , 0xBFD8     },
                    { AV_CRC_16_ANSI   , 0x8005    , 0x1FBB     },
                    { AV_CRC_8_ATM     , 0x07      , 0xE3       }
    };
