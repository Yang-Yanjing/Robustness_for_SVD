}
static bool default_msg_handler( decoder_t *p_dec, GstMessage *p_msg )
{
    bool err = false;
    switch( GST_MESSAGE_TYPE( p_msg ) ){
    case GST_MESSAGE_ERROR:
        {
            gchar  *psz_debug;
            GError *p_error;
            gst_message_parse_error( p_msg, &p_error, &psz_debug );
            g_free( psz_debug );
            msg_Err( p_dec, "Error from %s: %s",
                    GST_ELEMENT_NAME( GST_MESSAGE_SRC( p_msg ) ),
                    p_error->message );
            g_error_free( p_error );
            err = true;
        }
        break;
    case GST_MESSAGE_WARNING:
        {
            gchar  *psz_debug;
            GError *p_error;
            gst_message_parse_warning( p_msg, &p_error, &psz_debug );
            g_free( psz_debug );
            msg_Warn( p_dec, "Warning from %s: %s",
                    GST_ELEMENT_NAME( GST_MESSAGE_SRC( p_msg ) ),
                    p_error->message );
            g_error_free( p_error );
        }
        break;
    case GST_MESSAGE_INFO:
        {
            gchar  *psz_debug;
            GError *p_error;
            gst_message_parse_info( p_msg, &p_error, &psz_debug );
            g_free( psz_debug );
            msg_Info( p_dec, "Info from %s: %s",
                    GST_ELEMENT_NAME( GST_MESSAGE_SRC( p_msg ) ),
                    p_error->message );
            g_error_free( p_error );
        }
        break;
    default:
        break;
    }
    return err;
}
