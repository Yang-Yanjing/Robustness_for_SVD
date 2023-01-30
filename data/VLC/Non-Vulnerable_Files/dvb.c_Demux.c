/** Parses the whole channels.conf file */
static int Demux(demux_t *demux)
{
    input_item_t *input = GetCurrentItem(demux);
    input_item_node_t *subitems = input_item_node_Create(input);
    char *line;
    while ((line = stream_ReadLine(demux->s)) != NULL)
    {
        input_item_t *item = ParseLine(line);
        if (item == NULL)
            continue;
        input_item_node_AppendItem(subitems, item);
        vlc_gc_decref(item);
    }
    input_item_node_PostAndDelete(subitems);
    vlc_gc_decref(input);
    return 0; /* Needed for correct operation of go back */
}
