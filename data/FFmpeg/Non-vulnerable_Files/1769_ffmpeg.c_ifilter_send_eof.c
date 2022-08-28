static int ifilter_send_eof(InputFilter *ifilter)
{
    int i, j, ret;
    ifilter->eof = 1;
    if (ifilter->filter) {
        ret = av_buffersrc_add_frame_flags(ifilter->filter, NULL, AV_BUFFERSRC_FLAG_PUSH);
        if (ret < 0)
            return ret;
    } else {
        
        FilterGraph *fg = ifilter->graph;
        for (i = 0; i < fg->nb_inputs; i++)
            if (!fg->inputs[i]->eof)
                break;
        if (i == fg->nb_inputs) {
            
            
            
            for (j = 0; j < fg->nb_outputs; j++)
                finish_output_stream(fg->outputs[j]->ost);
        }
    }
    return 0;
}
