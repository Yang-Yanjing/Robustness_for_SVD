}
json_value * json_parse (const json_char * json)
{
   json_settings settings;
   memset (&settings, 0, sizeof (json_settings));
   return json_parse_ex (&settings, json, 0);
}
