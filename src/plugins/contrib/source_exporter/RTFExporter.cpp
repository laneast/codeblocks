/*
 * This code is based in the RTF output generated by Dev-C++
 *
 * The tests have shown some missing styles, namely style 11
 */

#include "RTFExporter.h"
#include <configmanager.h>
#include <wx/fontutil.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>

using std::ofstream;
using std::ostringstream;
using std::hex;
using std::setw;
using std::setfill;
using std::uppercase;
using std::size_t;
using std::find;
using std::for_each;

namespace
{
  // Helper function to convert i to a string
  inline string to_string(int i)
  {
    ostringstream ostr;

    ostr << i;

    return ostr.str();
  }

  // operator == overloaded for wxColour objects
  bool operator == (const wxColour &left, const wxColour &right)
  {
    return left.Red() == right.Red() && left.Green() == right.Green() && left.Blue() == right.Blue();
  }

  // operator != overloaded for wxColour objects
  bool operator != (const wxColour &left, const wxColour &right)
  {
    return left.Red() != right.Red() || left.Green() != right.Green() || left.Blue() != right.Blue();
  }

  // Helper class to append colors in RTF colortbl format to a string
  class wxColourToRTFTbl
  {
    private:
      string *_str;

    public:
      wxColourToRTFTbl(string *str)
      : _str(str)
      {
        //
      }

      void operator () (const wxColour &color)
      {
        (*_str) += string("\\red") + to_string(color.Red());
        (*_str) += string("\\green") + to_string(color.Green());
        (*_str) += string("\\blue") + to_string(color.Blue());
        (*_str) += string(";");
      }
  };
};

bool RTFExporter::Style::operator == (int aValue)
{
  return value == aValue;
}

string RTFExporter::RTFFontTable(int &pt)
{
  string fonttbl("{\\rtf1\\ansi\\deff0\\deftab720{\\fonttbl{\\f0\\fmodern ");
  wxString fontstring = ConfigManager::Get()->Read(_T("/editor/font"), wxEmptyString);
  pt = 8;

  if (!fontstring.IsEmpty())
  {
    wxFont tmpFont;
    wxNativeFontInfo nfi;
    nfi.FromString(fontstring);
    tmpFont.SetNativeFontInfo(nfi);

    pt = tmpFont.GetPointSize();
    wxString faceName = tmpFont.GetFaceName();

    if (!faceName.IsEmpty())
    {
      fonttbl += string(faceName.c_str());
    }
    else
    {
      fonttbl += "Courier New";
    }
  }
  else
  {
    fonttbl += "Courier New";
  }

  fonttbl += ";}}\n";

  return fonttbl;
}

string RTFExporter::RTFColorTable(const EditorColorSet *c_color_set, HighlightLanguage lang)
{
  string colortbl("{\\colortbl");
  vector<wxColour> color_tbl; // We'll store fore and back colors here

  m_styles.clear(); // Be sure the styles are cleared
  defStyleIdx = -1; // No default style

  if (lang != HL_NONE)
  {
    const int count = const_cast<EditorColorSet *>(c_color_set)->GetOptionCount(lang);

    for (int i = 0; i < count; ++i)
    {
      OptionColor *optc = const_cast<EditorColorSet *>(c_color_set)->GetOptionByIndex(lang, i);

      if (!optc->isStyle)
      {
        continue;
      }

      vector<wxColour>::iterator foreIter = find(color_tbl.begin(), color_tbl.end(), optc->fore);

      // Is the fore color already in color_tbl?
      if (foreIter == color_tbl.end())
      {
        // NO? Then push it and make foreIter point to it (it's the last one)
        color_tbl.push_back(optc->fore);
        foreIter = color_tbl.end() - 1;
      }

      int foreIdx = foreIter - color_tbl.begin();

      vector<wxColour>::iterator backIter = find(color_tbl.begin(), color_tbl.end(), optc->back);

      // Is the back color already in color_tbl?
      if (backIter == color_tbl.end())
      {
        // NO? Then push it and make backIter point to it (it's the last one)
        color_tbl.push_back(optc->back);
        backIter = color_tbl.end() - 1;
      }
      /* TODO (Ceniza#1#): If the background color isn't set don't add it (backIdx = -1) and check for it later */
      int backIdx = backIter - color_tbl.begin();

      Style tmpStyle =
      {
        optc->value,
        backIdx,
        foreIdx,
        optc->bold,
        optc->italics,
        optc->underlined
      };

      m_styles.push_back(tmpStyle);

      // Default Style
      if (optc->value == 0)
      {
        defStyleIdx = m_styles.size() - 1;
      }
    }

    // We've got all the colors and all styles. Write the colors
    for_each(color_tbl.begin(), color_tbl.end(), wxColourToRTFTbl(&colortbl));
  }

  colortbl += "}\n";

  return colortbl;
}

const char *RTFExporter::RTFInfo = "{\\info{\\comment Generated by the Code::Blocks RTF Exporter plugin}\n";
const char *RTFExporter::RTFTitle = "{\\title Untitled}}\n";

string RTFExporter::RTFBody(const wxMemoryBuffer &styled_text, int pt)
{
  string rtf_body("\n\\deflang1033\\pard\\plain\\f0");
  const char *buffer = reinterpret_cast<char *>(styled_text.GetData());
  const size_t buffer_size = styled_text.GetDataLen();

  rtf_body += string("\\fs") + to_string(pt * 2) + string(" ");

  if (buffer_size == 0)
  {
    return rtf_body;
  }

  // Get the current style from the first character
  char current_style = buffer[1];

  // If the first style happen to be the body style...
  if (current_style == 0)
  {
    if (defStyleIdx != -1)
    {
      vector<Style>::iterator i = m_styles.begin() + defStyleIdx;

      rtf_body += string("\\cb") + to_string(i->backIdx);
      rtf_body += string("\\cf") + to_string(i->foreIdx);

      if (i->bold)
      {
        rtf_body += "\\b";
      }

      if (i->italics)
      {
        rtf_body += "\\i";
      }

      if (i->underlined)
      {
        rtf_body += "\\ul";
      }

      rtf_body += " ";
    }
  }
  else
  {
    vector<Style>::iterator i = find(m_styles.begin(), m_styles.end(), current_style);

    if (i != m_styles.end())
    {
      rtf_body += string("\\cb") + to_string(i->backIdx);
      rtf_body += string("\\cf") + to_string(i->foreIdx);

      if (i->bold)
      {
        rtf_body += "\\b";
      }

      if (i->italics)
      {
        rtf_body += "\\i";
      }

      if (i->underlined)
      {
        rtf_body += "\\ul";
      }

      rtf_body += " ";
    }
  }

  for (size_t i = 0; i < buffer_size; i += 2)
  {
    if (buffer[i + 1] != current_style)
    {
      if (!isspace(buffer[i]))
      {
        vector<Style>::iterator oldStyle = find(m_styles.begin(), m_styles.end(), current_style);

        if (oldStyle != m_styles.end())
        {
          if (oldStyle->underlined)
          {
            rtf_body += "\\ul0";
          }

          if (oldStyle->italics)
          {
            rtf_body += "\\i0";
          }

          if (oldStyle->bold)
          {
            rtf_body += "\\b0";
          }
        }

        current_style = buffer[i + 1];

        vector<Style>::iterator newStyle = find(m_styles.begin(), m_styles.end(), current_style);

        if (newStyle != m_styles.end())
        {
          rtf_body += string("\\cb") + to_string(newStyle->backIdx);
          rtf_body += string("\\cf") + to_string(newStyle->foreIdx);

          if (newStyle->bold)
          {
            rtf_body += "\\b";
          }

          if (newStyle->italics)
          {
            rtf_body += "\\i";
          }

          if (newStyle->underlined)
          {
            rtf_body += "\\ul";
          }
        }
        else if (defStyleIdx != -1)
        {
          vector<Style>::iterator i = m_styles.begin() + defStyleIdx;

          rtf_body += string("\\cb") + to_string(i->backIdx);
          rtf_body += string("\\cf") + to_string(i->foreIdx);

          if (i->bold)
          {
            rtf_body += "\\b";
          }

          if (i->italics)
          {
            rtf_body += "\\i";
          }

          if (i->underlined)
          {
            rtf_body += "\\ul";
          }
        }

        rtf_body += " ";
      }
    }

    switch (buffer[i])
    {
      case '{':
        rtf_body += "\\{";
        break;

      case '}':
        rtf_body += "\\}";
        break;

      case '\r':
        break;

      case '\n':
        rtf_body += "\n\\par ";
        break;

      default:
        rtf_body += buffer[i];
        break;
    };
  }

  return rtf_body;
}

const char *RTFExporter::RTFEnd = "\n\\par }";

void RTFExporter::Export(const wxString &filename, const wxString &title, const wxMemoryBuffer &styled_text, const EditorColorSet *color_set)
{
  string rtf_code;
  HighlightLanguage lang = const_cast<EditorColorSet *>(color_set)->GetLanguageForFilename(title);
  int pt;

  rtf_code += RTFFontTable(pt);
  rtf_code += RTFColorTable(color_set, lang);
  rtf_code += RTFInfo;
  rtf_code += RTFTitle;
  rtf_code += RTFBody(styled_text, pt);
  rtf_code += RTFEnd;

  ofstream file(filename.c_str());
  file << rtf_code;
}
