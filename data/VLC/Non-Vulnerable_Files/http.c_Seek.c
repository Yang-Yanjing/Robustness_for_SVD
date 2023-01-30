 *****************************************************************************/
static int Seek( sout_access_out_t *p_access, off_t i_pos )
{
    (void)i_pos;
    msg_Warn( p_access, "HTTP sout access cannot seek" );
    return VLC_EGENERIC;
}
