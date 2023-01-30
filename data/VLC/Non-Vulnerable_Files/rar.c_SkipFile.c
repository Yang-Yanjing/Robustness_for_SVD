}
static int SkipFile(stream_t *s, int *count, rar_file_t ***file,
                    const rar_block_t *hdr, const char *volume_mrl)
{
    const uint8_t *peek;
    int min_size = 7+21;
    if (hdr->flags & RAR_BLOCK_FILE_HAS_HIGH)
        min_size += 8;
    if (hdr->size < (unsigned)min_size)
        return VLC_EGENERIC;
    if (stream_Peek(s, &peek, min_size) < min_size)
        return VLC_EGENERIC;
    /* */
    uint32_t file_size_low = GetDWLE(&peek[7+4]);
    uint8_t  method = peek[7+18];
    uint16_t name_size = GetWLE(&peek[7+19]);
    uint32_t file_size_high = 0;
    if (hdr->flags & RAR_BLOCK_FILE_HAS_HIGH)
        file_size_high = GetDWLE(&peek[7+29]);
    const uint64_t file_size = ((uint64_t)file_size_high << 32) | file_size_low;
    char *name = calloc(1, name_size + 1);
    if (!name)
        return VLC_EGENERIC;
    const int name_offset = (hdr->flags & RAR_BLOCK_FILE_HAS_HIGH) ? (7+33) : (7+25);
    if (name_offset + name_size <= hdr->size) {
        const int max_size = name_offset + name_size;
        if (stream_Peek(s, &peek, max_size) < max_size) {
            free(name);
            return VLC_EGENERIC;
        }
        memcpy(name, &peek[name_offset], name_size);
    }
    rar_file_t *current = NULL;
    if (method != 0x30) {
        msg_Dbg(s, "Ignoring compressed file %s (method=0x%2.2x)", name, method);
        goto exit;
    }
    /* */
    if( *count > 0 )
        current = (*file)[*count - 1];
    if (current &&
        (current->is_complete ||
          strcmp(current->name, name) ||
          (hdr->flags & RAR_BLOCK_FILE_HAS_PREVIOUS) == 0))
        current = NULL;
    if (!current) {
        if (hdr->flags & RAR_BLOCK_FILE_HAS_PREVIOUS)
            goto exit;
        current = malloc(sizeof(*current));
        if (!current)
            goto exit;
        TAB_APPEND(*count, *file, current);
        current->name = name;
        current->size = file_size;
        current->is_complete = false;
        current->real_size = 0;
        TAB_INIT(current->chunk_count, current->chunk);
        name = NULL;
    }
    /* Append chunks */
    rar_file_chunk_t *chunk = malloc(sizeof(*chunk));
    if (chunk) {
        chunk->mrl = strdup(volume_mrl);
        chunk->offset = stream_Tell(s) + hdr->size;
        chunk->size = hdr->add_size;
        chunk->cummulated_size = 0;
        if (current->chunk_count > 0) {
            rar_file_chunk_t *previous = current->chunk[current->chunk_count-1];
            chunk->cummulated_size += previous->cummulated_size +
                                      previous->size;
        }
        TAB_APPEND(current->chunk_count, current->chunk, chunk);
        current->real_size += hdr->add_size;
    }
    if ((hdr->flags & RAR_BLOCK_FILE_HAS_NEXT) == 0)
        current->is_complete = true;
exit:
    /* */
    free(name);
    /* We stop on the first non empty file if we cannot seek */
    if (current) {
        bool can_seek = false;
        stream_Control(s, STREAM_CAN_SEEK, &can_seek);
        if (!can_seek && current->size > 0)
            return VLC_EGENERIC;
    }
    if (SkipBlock(s, hdr))
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
