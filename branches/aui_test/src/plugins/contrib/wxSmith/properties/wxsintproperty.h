#ifndef WXSINTPROPERTY_H
#define WXSINTPROPERTY_H

#include "../wxsproperties.h"

class wxsIntProperty : public wxsProperty
{
	public:
        /** Ctor
         *
         * \param Int - variable which will be monitored
         * \param AlwaysUpdate - if true, any change (includig key events) will be
         *                       reported, if false, update will be made only after enter
         *                       or after loosing focus
         */
		wxsIntProperty(wxsProperties* Properties,int &Int, bool AlwaysUpdate);

		/** Dctor */
		virtual ~wxsIntProperty();

        /** Taking name of value type handled by this item */
        virtual const wxString& GetTypeName();

    protected:

        virtual void AddToPropGrid(wxPropertyGrid* Grid,const wxString& Name);
        virtual bool PropGridChanged(wxPropertyGrid* Grid,wxPGId Id);
        virtual void UpdatePropGrid(wxPropertyGrid* Grid);

        /** This function makes additional correction for value, must always
         *  return acceptable one. It can be declared inside derived classes
         *  to extend abilities of IntProperty (f.ex. inly odd numbers)
         */
        virtual int CorrectValue(int Value) { return Value; }

	private:

        int& Value;
        bool AlwUpd;

        wxPGId PGId;
};

#endif // WXSINTPROPERTY_H