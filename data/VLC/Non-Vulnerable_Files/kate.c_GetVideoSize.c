*/
static void GetVideoSize( decoder_t *p_dec, int *w, int *h )
{
    /* searching for vout to get its size is frowned upon, so we don't and
       use a default size if the original canvas size is not specified. */
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->ki.original_canvas_width > 0 && p_sys->ki.original_canvas_height > 0 )
    {
        *w = p_sys->ki.original_canvas_width;
        *h = p_sys->ki.original_canvas_height;
        msg_Dbg( p_dec, "original canvas %zu %zu",
	         p_sys->ki.original_canvas_width, p_sys->ki.original_canvas_height );
    }
    else
    {
        /* nothing, leave defaults */
        msg_Dbg( p_dec, "original canvas size unknown");
    }
}
