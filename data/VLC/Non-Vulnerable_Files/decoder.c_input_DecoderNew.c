 */
decoder_t *input_DecoderNew( input_thread_t *p_input,
                             es_format_t *fmt, input_clock_t *p_clock,
                             sout_instance_t *p_sout  )
{
    return decoder_New( VLC_OBJECT(p_input), p_input, fmt, p_clock,
                        p_input->p->p_resource, p_sout );
}
