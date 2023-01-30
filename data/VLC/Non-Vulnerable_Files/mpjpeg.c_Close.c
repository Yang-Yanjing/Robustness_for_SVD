 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    /* TODO: send the ending boundary ("\r\n--"BOUNDARY"--\r\n"),
     * but is the access_output still useable?? */
    msg_Dbg( p_this, "Multipart jpeg muxer closed" );
}
