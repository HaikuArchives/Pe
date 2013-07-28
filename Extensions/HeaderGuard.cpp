/*
 * HeaderGuard is an Extension.
 */

#include "PeAddOn.h"
#include <ctype.h>


#if __INTEL__
	extern "C" _EXPORT status_t perform_edit(MTextAddOn *addon);
#else
#	pragma export on
	extern "C"
		status_t perform_edit(MTextAddOn *addon);
#	pragma export reset
#endif
status_t perform_edit(MTextAddOn *addon)
{
	status_t result = B_OK;
	entry_ref headerFile;

	result = addon->GetRef(headerFile);

	if (result == B_OK)
	{
		BString fileName(headerFile.name);

		fileName.ReplaceSet(".+-", "_");
		fileName.Prepend("_");
		fileName.Append("_");

		// Do not change the case if a shift key was pressed
		if ((modifiers() & B_SHIFT_KEY) == 0)
			fileName.ToUpper();

		addon->Select(0, 0);

		BString insert;
		insert << "#ifndef " << fileName << "\n#define " << fileName << "\n\n";
		addon->Insert(insert.String());

		addon->Select(addon->TextLength(), addon->TextLength());

		insert = "\n#endif\t// ";
		insert.Append(fileName);
		insert.Append("\n");
		addon->Insert(insert.String());
	}
	
	return result;
}

