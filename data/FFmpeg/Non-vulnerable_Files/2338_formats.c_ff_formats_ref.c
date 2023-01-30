int ff_formats_ref(AVFilterFormats *f, AVFilterFormats **ref)
{
    FORMATS_REF(f, ref);
}
