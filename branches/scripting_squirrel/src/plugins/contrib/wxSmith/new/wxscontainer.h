#ifndef WXSCONTAINER_H
#define WXSCONTAINER_H

#include "wxsparent.h"
#include "wxsbaseproperties.h"

/** \brief Container is a class which represents widget that can
 *         have child items or one of root items
 */
class wxsContainer: public wxsParent
{
    public:

        /** \brief Ctor
         *  \param Resource resource containing this item
         *  \param BasePropertiesFlags flags filtering sed base properties
         *         (see wxsBaseProperties for details)
         *  \param Info pointer to static widget info
         *  \param EventArray pointer to static set of events
         *  \param StyleSet set of used styles, if NULL, this widget won't
         *         provide styles by default
         *  \param DefaultStyle default style used on read errors, string can
         *         contain one or more style names separated with '|' character
         */
        wxsContainer(
            wxsWindowRes* Resource,
            long BasePropertiesFlags,
            const wxsItemInfo* Info,
            const wxsEventDesc* EventArray = NULL,
            const wxsStyle* StyleSet=NULL,
            const wxString& DefaultStyle=wxEmptyString);

        /** \brief Function returning wxsBaseProperties object associated with
         *         this item.
         */
        virtual wxsBaseProperties* GetBaseProps() { return &BaseProps; }

        /** \brief Function returning flags filtering base properties */
        inline long GetBasePropertiesFlags() { return BasePropertiesFlags; }

    protected:

        /** \brief container with base properties */
        wxsBaseProperties BaseProps;

        /** \brief Function enumerating properties for this container only
         *
         * This function should enumerate all extra properties
         * required by item.
         * These properties will be placed at the beginning, right after
         * there will be Variable name and identifier and at the end, all
         * required base properties.
         */
        virtual void EnumContainerProperties(long Flags) = 0;

        /** \brief Function which adding new items to QPP
         *
         * This function may be used to add special quick properties for
         * this item. Use this function instead of AddItemProperties
         * because there is standard QPP added by wdget itselt.
         *
         * All QPPChild panels will be added before additional panels
         * added by widget.
         */
        virtual void AddContainerQPP(wxsAdvQPP* QPP) { }

        /** \brief Function enumerating properties with default ones
         *
         * Function enumerating item properties. The implementation
         * does call EnumContainerProperties() and adds all default properties.
         * in usual case this should not be overridden, all properties should
         * be added using EnumContainerProperties funcion. In some advanced
         * cases, this may take place.
         */
        virtual void EnumItemProperties(long Flags);

        /** \brief Function Adding QPPChild panels for base properties of this
         *         container.
         *
         * This function calls internally AddContainerQPP to add any additional
         * QPPChild panels. If you want to add panel for specified widget,
         * use AddContainerQPP rather than AddItemQPP.
         */
        virtual void AddItemQPP(wxsAdvQPP* QPP);

        /** \brief Checking if can add child item
         *
         * This function bases on rule that container can have
         * either one sizer child or set of items which are not sizer / spacer.
         */
        virtual bool CanAddChild(wxsItem* Item,bool ShowMessage);

        /** \brief Easy access to position */
        inline wxPoint Pos(wxWindow* Parent)
        {
            return BaseProps.Position.GetPosition(Parent);
        }

        /** \brief Easy access to size */
        inline wxSize Size(wxWindow* Parent)
        {
            return BaseProps.Size.GetSize(Parent);
        }

        /** \brief Easy access to style (can be used directly in wxWidgets */
        inline long Style()
        {
            return wxsStyleProperty::GetWxStyle(StyleBits,StyleSet,false);
        }

        /** \brief Function setting up standard widget properties after
         *         the window is created.
         *
         * Included properties:
         *  - Enabled
         *  - Focused
         *  - Hidden (skipped when not exact preview)
         *  - FG - Foreground colour
         *  - BG - Background colour
         *  - Font
         *  - ToolTip
         *  - HelpText
         *  - Extra style
         *
         * \param Preview window for which properties must be applied
         * \param IsExact value of IsExact argument passed to BuildPreview
         * \return window after settig up properties (value of Preview is returned)
         */
        wxWindow* SetupWindow(wxWindow* Preview,bool IsExact);

        /** \brief Easy acces to position code */
        inline wxString PosCode(const wxString& Parent,wxsCodingLang Language)
        {
            return BaseProps.Position.GetPositionCode(Parent,Language);
        }

        /** \brief Easy acces to size code */
        inline wxString SizeCode(const wxString& Parent,wxsCodingLang Language)
        {
            return BaseProps.Size.GetSizeCode(Parent,Language);
        }

        /** \brief Easy access to style code */
        inline wxString StyleCode(wxsCodingLang Language)
        {
            return wxsStyleProperty::GetString(StyleBits,StyleSet,false,Language);
        }

        /** \brief Function adding code setting up properties after window
         *         is created.
         *
         * Behaviour of this function is simillar to SetupWindow but this
         * one creates source code doing it instead of setting up real window
         */
        void SetupWindowCode(wxString& Code,wxsCodingLang Language);

        /** \brief Function adding children items into preview window */
        void AddChildrenPreview(wxWindow* This,bool Exact);

        /** \brief Function adding code generating child items */
        void AddChildrenCode(wxString& Code,wxsCodingLang Language);

    private:

        /** \brief Widget info retrieval won't be accessed from derived classes,
         *         info must be passed to wxsWidget's constructor
         */
        virtual const wxsItemInfo& GetInfo() { return *Info; }

        /** \brief Event array must be passed into constructor */
        virtual const wxsEventDesc* GetEventArray() { return EventArray; }

        const wxsItemInfo* Info;
        const wxsEventDesc* EventArray;
        const wxsStyle* StyleSet;
        wxString DefaultStyle;
        long StyleBits;
        long ExStyleBits;
        long BasePropertiesFlags;
};

#endif