static const int rar_marker_size = sizeof(rar_marker);
void RarFileDelete(rar_file_t *file)
{
    for (int i = 0; i < file->chunk_count; i++) {
        free(file->chunk[i]->mrl);
        free(file->chunk[i]);
    }
    free(file->chunk);
    free(file->name);
    free(file);
}
