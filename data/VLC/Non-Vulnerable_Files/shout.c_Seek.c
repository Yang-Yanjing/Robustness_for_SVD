 *****************************************************************************/
static int Seek( sout_access_out_t *p_access, off_t i_pos )
{
    VLC_UNUSED(i_pos);
    msg_Err( p_access, "cannot seek on shout" );
    return VLC_EGENERIC;
}
