/*
 * HeaderGuard is an Extension.
 */

#include "PeAddOn.h"
#include <ctype.h>


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
	entry_ref headerFile;
	
	result = addon->GetRef (headerFile);
	
	if (result == B_OK)
	{
		BString fileName(headerFile.name);

		fileName.ReplaceAll (".", "_");
		fileName.Prepend ("_");
		fileName.Append ("_");
		
		addon->Select (0,0);
		
		BString ifdefString;
		ifdefString << "#ifndef " << fileName << "\n#define " << fileName << "\n\n";
	
		addon->Insert (ifdefString.String());
	
		addon->Select(addon->TextLength (), addon->TextLength ());
	
		addon->Insert ("#endif\n");
	}
	
	return result;
}

