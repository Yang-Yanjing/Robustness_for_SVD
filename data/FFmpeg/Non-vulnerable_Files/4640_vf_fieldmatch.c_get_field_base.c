static int get_field_base(int match, int field)
{
    return match < 3 ? 2 - field : 1 + field;
}
