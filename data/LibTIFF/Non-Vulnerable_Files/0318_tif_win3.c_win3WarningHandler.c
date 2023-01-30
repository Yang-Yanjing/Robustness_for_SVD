static void
win3WarningHandler(const char* module, const char* fmt, va_list ap)
{
	char e[512] = { '\0' };
	if (module != NULL)
		strcat(strcpy(e, module), ":");
	vsprintf(e+strlen(e), fmt, ap);
	strcat(e, ".");
	MessageBox(GetActiveWindow(), e, "LibTIFF Warning",
	    MB_OK|MB_ICONEXCLAMATION);
}
