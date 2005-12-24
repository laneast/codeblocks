#ifndef TOKEN_H
#define TOKEN_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/file.h>
#include <settings.h>

#include "blockallocated.h"

#include "searchtree.h"

class Token;
WX_DEFINE_ARRAY(Token*, TokensArray);

enum TokenScope
{
	tsUndefined = 0,
	tsPrivate,
	tsProtected,
	tsPublic
};

enum TokenKind
{
	tkClass         = 0x0001,
	tkNamespace     = 0x0002,
	tkConstructor   = 0x0004,
	tkDestructor    = 0x0008,
	tkFunction      = 0x0010,
	tkVariable      = 0x0020,
	tkEnum          = 0x0040,
	tkEnumerator    = 0x0080,
	tkPreprocessor  = 0x0100,
	tkUndefined     = 0xFFFF,
};


class Token  : public BlockAllocated<Token, 10000>
{
	public:
		Token();
		Token(const wxString& name, const wxString& filename, unsigned int line);
		~Token();

		void AddChild(Token* child);
		wxString GetNamespace() const;
		bool InheritsFrom(Token* token) const;
		wxString GetTokenKindString() const;
		wxString GetTokenScopeString() const;

		bool SerializeIn(wxInputStream* f);
		bool SerializeOut(wxOutputStream* f);

		wxString m_Type; // this is the return value (if any): e.g. const wxString&
		wxString m_ActualType; // this is what the parser believes is the actual return value: e.g. wxString
		wxString m_Name;
		wxString m_DisplayName;
		wxString m_Args;
		wxString m_AncestorsString; // all ancestors comma-separated list
		wxString m_Filename;
		unsigned int m_Line;
		wxString m_ImplFilename;
		unsigned int m_ImplLine;
		TokenScope m_Scope;
		TokenKind m_TokenKind;
		bool m_IsOperator;
		bool m_IsLocal; // found in a local file?
		bool m_IsTemporary;
		TokensArray m_Ancestors; // after Parser::LinkInheritance() runs, contains all ancestor tokens (affected by m_AncestorsString)

		wxString m_String; // custom string value
		bool m_Bool; // custom bool value
		int m_Int; // custom int value
		void* m_Data; // custom pointer value

		Token* m_pParent;
		TokensArray m_Children;

        // helper variables used in serialization
        int m_ParentIndex;
        wxArrayInt m_AncestorsIndices;
        wxArrayInt m_ChildrenIndices;
	protected:
	private:
};

inline void SaveIntToFile(wxOutputStream* f, int i)
{
    /* This used to be done as
        f->Write(&i, sizeof(int));
    which is incorrect because it assumes a consistant byte order
    and a constant int size */

    unsigned int const j = i; // rshifts aren't well-defined for negatives
    unsigned char c[4] = { j>>0&0xFF, j>>8&0xFF, j>>16&0xFF, j>>24&0xFF };
    f->Write( c, 4 );
}

inline bool LoadIntFromFile(wxInputStream* f, int* i)
{
//    See SaveIntToFile
//    return f->Read(i, sizeof(int)) == sizeof(int);

    unsigned char c[4];
    if ( f->Read( c, 4 ).LastRead() != 4 ) return false;
    *i = ( c[0]<<0 | c[1]<<8 | c[2]<<16 | c[3]<<24 );
    return true;
}

inline void SaveStringToFile(wxOutputStream* f, const wxString& str)
{
    const wxWX2MBbuf psz = str.mb_str(wxConvUTF8);
    int size = psz ? strlen(psz) : 0;
    if (size >= 512)
        size = 512;
    SaveIntToFile(f, size);
    if(size)
        f->Write(psz, size);
}

inline bool LoadStringFromFile(wxInputStream* f, wxString& str)
{
    int size;
    if (!LoadIntFromFile(f, &size))
        return false;
    bool ok = true;
    if (size > 0 && size <= 512)
    {
        static char buf[513];
        ok = f->Read(buf, size).LastRead() == (size_t)size;
        buf[size] = '\0';
        str = wxString(buf, wxConvUTF8);
    }
    else // doesn't fit in our buffer, but still we have to skip it
    {
        str.Empty();
        size = size & 0xFFFFFF; // Can't get any longer than that
        f->SeekI(size, wxFromCurrent);
    }
    return ok;
}

typedef SearchTree<TokensArray> TokenSearchTree;
class TokensTree
{
    public:
        TokenSearchTree m_Tree;
//        size_t GetHash(const wxString& name);
        void Clear();
        TokensTree();
        Token* TokenExists(const wxString& name, Token* parent, short int kindMask);
        void AddToken(const wxString& name,Token* newToken);
        virtual ~TokensTree() {}
//    private:
//        unsigned int m_hash1[256],m_hash2[256];
};

#endif // TOKEN_H

