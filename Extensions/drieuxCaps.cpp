/*
 * drieuxCaps is based on EducateQuotes.
 *
 * This is a real quick'n'dirty hack.  I thought of it about two minutes ago;
 * with any luck I'll be done in another ten.  - Jamie, 6:43 PM, 5 May 93.
 *
 * Took longer than I thought.  I didn't have a copy of JRandom up here at
 * work, so I had to redownload it from sumex-aim.  Sigh.  - 7:27 PM
 *
 *
 * MLH: Changed the code to turn it into an Pe addon
 *
 */

#include "PeAddOn.h"
#include <ctype.h>
#include "JRandom.h"

#if __INTEL__
	extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#	pragma export on
	extern "C"
		long perform_edit(MTextAddOn *addon);
#	pragma export reset
#endif

long perform_edit(MTextAddOn *addon)
{
	long result = B_OK;
	long s, e, len;
	unsigned char *r, *t;
	
	addon->GetSelection(&s, &e);
	
	if (s == e)
	{
		s = 0;
		e = addon->TextLength();
		addon->Select(s, e);
	}
	
	len = e - s;
	
	r = (unsigned char *)malloc(len);

	if (r)
	{
		memcpy(r, addon->Text() + s, len);

		jrStruct theJRStruct;
		IJRandom(&theJRStruct);
		
		bool changed = false;
		unsigned char *p = r;
		
		for (int i = 0; i < len; i++)
		{
			unsigned char c = *p;
			
			if ((i == 0) || (isspace(*(p-1)) || ispunct(*(p-1)))) //] & (__whitespace|__space_char|__punctuation)))
			{
				if (jrLinearShort(&theJRStruct, 0, 1)) {
					if (islower(c)) {
						c ^= 0x20;
					}
				} else {
					if (isupper(c)) {
						c ^= 0x20;
					}
				}
			}
			
			if (c != *p)
			{
				*p = c;
				changed = true;
			}
			
			++p;
		}
		
		if (changed)
		{
			addon->Delete();
			addon->Insert((char *)r, len);
		}
		
		free(r);
	}
	else
		result = B_ERROR;
	
	return result;
}





