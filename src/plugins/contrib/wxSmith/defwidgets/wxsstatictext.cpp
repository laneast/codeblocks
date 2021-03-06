#include "../wxsheaders.h"
#include "wxsstatictext.h"

#include "wxsstdmanager.h"
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/settings.h>
#include <wx/log.h>
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsStaticTextStyles)
    WXS_ST_CATEGORY("wxStaticText")
    WXS_ST(wxALIGN_RIGHT)
    WXS_ST(wxALIGN_CENTRE)
    WXS_ST(wxST_NO_AUTORESIZE)
    WXS_ST_DEFAULTS()
WXS_ST_END(wxsStaticTextStyles)

WXS_EV_BEGIN(wxsStaticTextEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsStaticTextEvents)

wxsDWDefineBegin(wxsStaticText,wxStaticText,
    ThisWidget = new wxStaticText(parent,id,label,pos,size,style);
    )
    wxsDWDefLongStr(label,"Label:","Label");
wxsDWDefineEnd()

