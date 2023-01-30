#define MAX_SHADER_FILE_SIZE 1024*1024
static int Direct3DCreateShaders(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->hd3d9x_dll)
        return VLC_EGENERIC;
    /* Find which shader was selected in the list. */
    char *selected_shader = var_InheritString(vd, "direct3d-shader");
    if (!selected_shader)
        return VLC_SUCCESS; /* Nothing to do */
    const char *shader_source_builtin = NULL;
    char *shader_source_file = NULL;
    FILE *fs = NULL;
    for (size_t i = 0; i < BUILTIN_SHADERS_COUNT; ++i) {
        if (!strcmp(selected_shader, builtin_shaders[i].name)) {
            shader_source_builtin = builtin_shaders[i].code;
            break;
        }
    }
    if (shader_source_builtin) {
        /* A builtin shader was selected. */
        int err = Direct3DCompileShader(vd, shader_source_builtin, strlen(shader_source_builtin));
        if (err)
            goto error;
    } else {
        if (strcmp(selected_shader, SELECTED_SHADER_FILE))
            goto error; /* Unrecognized entry in the list. */
        /* The source code of the shader needs to be read from a file. */
        char *filepath = var_InheritString(vd, "direct3d-shader-file");
        if (!filepath || !*filepath)
        {
            free(filepath);
            goto error;
        }
        /* Open file, find its size with fseek/ftell and read its content in a buffer. */
        fs = fopen(filepath, "rb");
        if (!fs)
            goto error;
        int ret = fseek(fs, 0, SEEK_END);
        if (ret == -1)
            goto error;
        long length = ftell(fs);
        if (length == -1 || length >= MAX_SHADER_FILE_SIZE)
            goto error;
        rewind(fs);
        shader_source_file = malloc(sizeof(*shader_source_file) * length);
        if (!shader_source_file)
            goto error;
        ret = fread(shader_source_file, length, 1, fs);
        if (ret != 1)
            goto error;
        ret = Direct3DCompileShader(vd, shader_source_file, length);
        if (ret)
            goto error;
    }
    free(selected_shader);
    free(shader_source_file);
    fclose(fs);
    return VLC_SUCCESS;
error:
    Direct3DDestroyShaders(vd);
    free(selected_shader);
    free(shader_source_file);
    if (fs)
        fclose(fs);
    return VLC_EGENERIC;
}
