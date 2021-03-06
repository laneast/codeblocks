#ifndef MSVCWORKSPACELOADER_H
#define MSVCWORKSPACELOADER_H

#include "ibaseworkspaceloader.h"
#include "msvcworkspacebase.h"

class MSVCWorkspaceLoader : public IBaseWorkspaceLoader, public MSVCWorkspaceBase
{
    public:
		MSVCWorkspaceLoader();
		virtual ~MSVCWorkspaceLoader();

        bool Open(const wxString& filename);
        bool Save(const wxString& title, const wxString& filename);
};

#endif // MSVCWORKSPACELOADER_H
