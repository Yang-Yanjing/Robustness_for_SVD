}
static void SetupText( decoder_t *p_dec, subpicture_t *p_spu, const kate_event *ev )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( ev->text_encoding != kate_utf8 )
    {
        msg_Warn( p_dec, "Text isn't UTF-8, unsupported, ignored" );
        return;
    }
    switch( ev->text_markup_type )
    {
        case kate_markup_none:
            p_spu->p_region->psz_text = strdup( ev->text ); /* no leak, this actually gets killed by the core */
            break;
        case kate_markup_simple:
            if( p_sys->b_formatted )
            {
                /* the HTML renderer expects a top level text tag pair */
                char *buffer = NULL;
                if( asprintf( &buffer, "<text>%s</text>", ev->text ) >= 0 )
                {
                    p_spu->p_region->psz_html = buffer;
                }
                break;
            }
            /* if not formatted, we fall through */
        default:
            /* we don't know about this one, so remove markup and display as text */
            {
                char *copy = strdup( ev->text );
                size_t len0 = strlen( copy ) + 1;
                kate_text_remove_markup( ev->text_encoding, copy, &len0 );
                p_spu->p_region->psz_text = copy;
            }
            break;
    }
}
