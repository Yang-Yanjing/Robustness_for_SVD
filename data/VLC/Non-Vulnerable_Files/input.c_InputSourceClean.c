 *****************************************************************************/
static void InputSourceClean( input_source_t *in )
{
    int i;
    if( in->p_demux )
        demux_Delete( in->p_demux );
    if( in->i_title > 0 )
    {
        for( i = 0; i < in->i_title; i++ )
            vlc_input_title_Delete( in->title[i] );
        TAB_CLEAN( in->i_title, in->title );
    }
}
