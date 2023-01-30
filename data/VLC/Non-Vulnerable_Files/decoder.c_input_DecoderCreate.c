 */
decoder_t *input_DecoderCreate( vlc_object_t *p_parent, es_format_t *fmt,
                                input_resource_t *p_resource )
{
    return decoder_New( p_parent, NULL, fmt, NULL, p_resource, NULL );
}
