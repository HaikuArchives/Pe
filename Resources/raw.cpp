main ()
{
	BApplication app("application/x-hekkel-raw");
	
	while (!feof(stdin))
	{
		rgb_color c = { 0, 0, 0, 0 };
		fread(&c.red, 1, 1, stdin);
		fread(&c.green, 1, 1, stdin);
		fread(&c.blue, 1, 1, stdin);
		
		BScreen s;
		char ic = s.IndexForColor(c);
		fwrite(&ic, 1, 1, stdout);
	}
}