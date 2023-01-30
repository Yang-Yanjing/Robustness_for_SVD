static void opengl_make_ortho(float matrix[16], float left, float right,
                              float bottom, float top, float nearZ, float farZ)
{
    float ral = right + left;
    float rsl = right - left;
    float tab = top + bottom;
    float tsb = top - bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    memset(matrix, 0, 16 * sizeof(float));
    matrix[0] = 2.0f / rsl;
    matrix[5] = 2.0f / tsb;
    matrix[10] = -2.0f / fsn;
    matrix[12] = -ral / rsl;
    matrix[13] = -tab / tsb;
    matrix[14] = -fan / fsn;
    matrix[15] = 1.0f;
}
