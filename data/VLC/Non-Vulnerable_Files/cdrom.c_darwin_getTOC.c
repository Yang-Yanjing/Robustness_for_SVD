 ****************************************************************************/
static CDTOC *darwin_getTOC( vlc_object_t * p_this, const vcddev_t *p_vcddev )
{
    mach_port_t port;
    char *psz_devname;
    kern_return_t ret;
    CDTOC *pTOC = NULL;
    io_iterator_t iterator;
    io_registry_entry_t service;
    CFMutableDictionaryRef properties;
    CFDataRef data;
    /* get the device name */
    if( ( psz_devname = strrchr( p_vcddev->psz_dev, '/') ) != NULL )
        ++psz_devname;
    else
        psz_devname = p_vcddev->psz_dev;
    /* unraw the device name */
    if( *psz_devname == 'r' )
        ++psz_devname;
    /* get port for IOKit communication */
    if( ( ret = IOMasterPort( MACH_PORT_NULL, &port ) ) != KERN_SUCCESS )
    {
        msg_Err( p_this, "IOMasterPort: 0x%08x", ret );
        return( NULL );
    }
    /* get service iterator for the device */
    if( ( ret = IOServiceGetMatchingServices(
                    port, IOBSDNameMatching( port, 0, psz_devname ),
                    &iterator ) ) != KERN_SUCCESS )
    {
        msg_Err( p_this, "IOServiceGetMatchingServices: 0x%08x", ret );
        return( NULL );
    }
    /* first service */
    service = IOIteratorNext( iterator );
    IOObjectRelease( iterator );
    /* search for kIOCDMediaClass */
    while( service && !IOObjectConformsTo( service, kIOCDMediaClass ) )
    {
        if( ( ret = IORegistryEntryGetParentIterator( service,
                        kIOServicePlane, &iterator ) ) != KERN_SUCCESS )
        {
            msg_Err( p_this, "IORegistryEntryGetParentIterator: 0x%08x", ret );
            IOObjectRelease( service );
            return( NULL );
        }
        IOObjectRelease( service );
        service = IOIteratorNext( iterator );
        IOObjectRelease( iterator );
    }
    if( !service )
    {
        msg_Err( p_this, "search for kIOCDMediaClass came up empty" );
        return( NULL );
    }
    /* create a CF dictionary containing the TOC */
    if( ( ret = IORegistryEntryCreateCFProperties( service, &properties,
                    kCFAllocatorDefault, kNilOptions ) ) != KERN_SUCCESS )
    {
        msg_Err( p_this, "IORegistryEntryCreateCFProperties: 0x%08x", ret );
        IOObjectRelease( service );
        return( NULL );
    }
    /* get the TOC from the dictionary */
    if( ( data = (CFDataRef) CFDictionaryGetValue( properties,
                                    CFSTR(kIOCDMediaTOCKey) ) ) != NULL )
    {
        CFRange range;
        CFIndex buf_len;
        buf_len = CFDataGetLength( data ) + 1;
        range = CFRangeMake( 0, buf_len );
        if( ( pTOC = malloc( buf_len ) ) != NULL )
        {
            CFDataGetBytes( data, range, (u_char *)pTOC );
        }
    }
    else
    {
        msg_Err( p_this, "CFDictionaryGetValue failed" );
    }
    CFRelease( properties );
    IOObjectRelease( service );
    return( pTOC );
}
