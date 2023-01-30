}
static int ControlGetReducedIndexLocked( input_thread_t *p_input )
{
    const int i_lt = p_input->p->control[0].i_type;
    int i;
    for( i = 1; i < p_input->p->i_control; i++ )
    {
        const int i_ct = p_input->p->control[i].i_type;
        if( i_lt == i_ct &&
            ( i_ct == INPUT_CONTROL_SET_STATE ||
              i_ct == INPUT_CONTROL_SET_RATE ||
              i_ct == INPUT_CONTROL_SET_POSITION ||
              i_ct == INPUT_CONTROL_SET_TIME ||
              i_ct == INPUT_CONTROL_SET_PROGRAM ||
              i_ct == INPUT_CONTROL_SET_TITLE ||
              i_ct == INPUT_CONTROL_SET_SEEKPOINT ||
              i_ct == INPUT_CONTROL_SET_BOOKMARK ) )
        {
            continue;
        }
        else
        {
            /* TODO but that's not that important
                - merge SET_X with SET_X_CMD
                - ignore SET_SEEKPOINT/SET_POSITION/SET_TIME before a SET_TITLE
                - ignore SET_SEEKPOINT/SET_POSITION/SET_TIME before another among them
                - ?
                */
            break;
        }
    }
    return i - 1;
}
