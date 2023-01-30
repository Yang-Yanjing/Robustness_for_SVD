}
static int scan_service_type( int service_type )
{
    switch( service_type )
    {
    case 0x01: return SERVICE_DIGITAL_TELEVISION; break;
    case 0x02: return SERVICE_DIGITAL_RADIO; break;
    case 0x16: return SERVICE_DIGITAL_TELEVISION_AC_SD; break;
    case 0x19: return SERVICE_DIGITAL_TELEVISION_AC_HD; break;
    default:   return SERVICE_UNKNOWN; break;
    }
}
