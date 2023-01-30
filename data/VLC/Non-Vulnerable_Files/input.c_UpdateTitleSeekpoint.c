 *****************************************************************************/
static int UpdateTitleSeekpoint( input_thread_t *p_input,
                                 int i_title, int i_seekpoint )
{
    int i_title_end = p_input->p->input.i_title_end -
                        p_input->p->input.i_title_offset;
    int i_seekpoint_end = p_input->p->input.i_seekpoint_end -
                            p_input->p->input.i_seekpoint_offset;
    if( i_title_end >= 0 && i_seekpoint_end >= 0 )
    {
        if( i_title > i_title_end ||
            ( i_title == i_title_end && i_seekpoint > i_seekpoint_end ) )
            return 0;
    }
    else if( i_seekpoint_end >= 0 )
    {
        if( i_seekpoint > i_seekpoint_end )
            return 0;
    }
    else if( i_title_end >= 0 )
    {
        if( i_title > i_title_end )
            return 0;
    }
    return 1;
}
