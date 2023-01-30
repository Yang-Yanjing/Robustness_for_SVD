 *****************************************************************************/
static char *MMIGetText( cam_t *p_cam, uint8_t **pp_apdu, int *pi_size )
{
    int i_tag = APDUGetTag( *pp_apdu, *pi_size );
    int l;
    uint8_t *d;
    if ( i_tag != AOT_TEXT_LAST )
    {
        msg_Err( p_cam->obj, "unexpected text tag: %06x", i_tag );
        *pi_size = 0;
        return strdup( "" );
    }
    d = APDUGetLength( *pp_apdu, &l );
    *pp_apdu += l + 4;
    *pi_size -= l + 4;
    return vlc_from_EIT(d,l);
}
