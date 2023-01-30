}
static int segment_RestorePos(segment_t *segment)
{
    if (segment->data)
    {
        uint64_t size = segment->size - segment->data->i_buffer;
        if (size > 0)
        {
            segment->data->i_buffer += size;
            segment->data->p_buffer -= size;
        }
    }
    return VLC_SUCCESS;
}
