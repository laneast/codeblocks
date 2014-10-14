/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "nativeparser.h"
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <pluginmanager.h>
#include <messagemanager.h>
#include <editormanager.h>
#include <customvars.h>
#include <cbeditor.h>
#include <cbproject.h>
#include "classbrowser.h"
#include "parser/parser.h"
#include <compilerfactory.h>

BEGIN_EVENT_TABLE(NativeParser, wxEvtHandler)
	EVT_MENU(THREAD_START, NativeParser::OnThreadStart)
	EVT_MENU(THREAD_END, NativeParser::OnThreadEnd)
	EVT_MENU(PARSER_END, NativeParser::OnParserEnd)
END_EVENT_TABLE()

NativeParser::NativeParser()
	: m_Parsers(1)
{
	//ctor
    m_pClassBrowser = 0L;
}

NativeParser::~NativeParser()
{
	RemoveClassBrowser();
	ClearParsers();
}

void NativeParser::CreateClassBrowser()
{
	if (!m_pClassBrowser)
		m_pClassBrowser = new ClassBrowser(Manager::Get()->GetNotebook(), this);
}

void NativeParser::RemoveClassBrowser(bool appShutDown)
{
    if (!appShutDown && m_pClassBrowser)
    {
        delete m_pClassBrowser;
    }
    m_pClassBrowser = 0L;
}

void NativeParser::RereadParserOptions()
{
	bool needsReparsing = false;
	for (ParsersMap::iterator it = m_Parsers.begin(); it != m_Parsers.end(); ++it)
	{
		Parser* parser = it->second;
		if (parser)
		{
			ParserOptions opts = parser->Options();
			parser->ReadOptions();
			if (opts.followLocalIncludes != parser->Options().followLocalIncludes ||
				opts.followGlobalIncludes != parser->Options().followGlobalIncludes ||
				opts.wantPreprocessor != parser->Options().wantPreprocessor)
			{
				// important options changed... flag for reparsing
				needsReparsing = true;
			}
		}
	}
	if (needsReparsing)
	{
		if (wxMessageBox(_("You changed some class parser options. Do you want to "
						"reparse your projects now, using the new options?"),
						_("Reparse?"),
						wxYES | wxNO | wxICON_QUESTION) == wxYES)
		{
			cbProject* proj = Manager::Get()->GetProjectManager()->GetActiveProject();
			ClearParsers();
			ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
			for (unsigned int i = 0; i < projects->GetCount(); ++i)
			{
				AddParser(projects->Item(i));
			}
			if (m_pClassBrowser)
				m_pClassBrowser->SetParser(m_Parsers[proj]);
		}
	}
	if (m_pClassBrowser)
		m_pClassBrowser->Update();
}

void NativeParser::SetClassBrowserProject(cbProject* project)
{
    if (m_pClassBrowser)
		m_pClassBrowser->SetParser(m_Parsers[project]);
}

void NativeParser::SetCBViewMode(const BrowserViewMode& mode)
{
	for (ParsersMap::iterator it = m_Parsers.begin(); it != m_Parsers.end(); ++it)
	{
		Parser* parser = it->second;
		if (parser)
		{
			parser->ClassBrowserOptions().showInheritance = mode == bvmInheritance;
		}
	}
	if (m_pClassBrowser)
		m_pClassBrowser->Update();
}

void NativeParser::ClearParsers()
{
	if (m_pClassBrowser)
		m_pClassBrowser->SetParser(0L);
	for (ParsersMap::iterator it = m_Parsers.begin(); it != m_Parsers.end(); ++it)
	{
		Parser* parser = it->second;
		if (parser)
		{
			delete parser;
        }
	}
	m_Parsers.clear();
}

void NativeParser::AddCompilerDirs(Parser* parser, cbProject* project)
{
	if (!parser)
		return;

    parser->IncludeDirs().Clear();
    wxString base = project->GetBasePath();

    Compiler* compiler = 0;
    // apply compiler global vars
	if (CompilerFactory::Compilers.GetCount() > 0 && CompilerFactory::CompilerIndexOK(project->GetCompilerIndex()))
	{
		compiler = CompilerFactory::Compilers[project->GetCompilerIndex()];
        compiler->GetCustomVars().ApplyVarsToEnvironment();
	}
    // apply project vars
    project->GetCustomVars().ApplyVarsToEnvironment();

    // get project include dirs
    for (unsigned int i = 0; i < project->GetIncludeDirs().GetCount(); ++i)
    {
        wxFileName dir(project->GetIncludeDirs()[i]);
        wxLogNull ln; // hide the error log about "too many ..", if the relative path is invalid
        if (!dir.IsAbsolute())
            dir.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
        if (dir.IsOk() && parser->IncludeDirs().Index(dir.GetFullPath()) == wxNOT_FOUND)
        {
            parser->IncludeDirs().Add(dir.GetFullPath());
//            Manager::Get()->GetMessageManager()->DebugLog("Parser prj dir: " + dir.GetFullPath());
        }
    }
    
    // get targets include dirs
    for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
    {
        ProjectBuildTarget* target = project->GetBuildTarget(i);
        if (target)
        {
        	// apply target vars
            target->GetCustomVars().ApplyVarsToEnvironment();
            for (unsigned int ti = 0; ti < target->GetIncludeDirs().GetCount(); ++ti)
            {
                wxFileName dir(target->GetIncludeDirs()[ti]);
                wxLogNull ln; // hide the error log about "too many ..", if the relative path is invalid
                if (!dir.IsAbsolute())
                    dir.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
                if (dir.IsOk() && parser->IncludeDirs().Index(dir.GetFullPath()) == wxNOT_FOUND)
                {
                    parser->IncludeDirs().Add(dir.GetFullPath());
//                    Manager::Get()->GetMessageManager()->DebugLog("Parser tgt dir: " + dir.GetFullPath());
                }
            }
        }
    }

    // add compiler include dirs
	if (compiler)
	{
		const wxArrayString& dirs = compiler->GetIncludeDirs();
		for (unsigned int i = 0; i < dirs.GetCount(); ++i)
		{
			//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "Adding %s", dirs[i].c_str());
            wxFileName dir(dirs[i]);
            wxLogNull ln; // hide the error log about "too many ..", if the relative path is invalid
            if (!dir.IsAbsolute())
                dir.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
            if (dir.IsOk() && parser->IncludeDirs().Index(dir.GetFullPath()) == wxNOT_FOUND)
            {
                parser->IncludeDirs().Add(dir.GetFullPath());
//                Manager::Get()->GetMessageManager()->DebugLog("Parser cmp dir: " + dir.GetFullPath());
            }
		}
	}
	else
		Manager::Get()->GetMessageManager()->DebugLog(_("No compilers found!"));
}

void NativeParser::AddParser(cbProject* project, bool useCache)
{
	if (!project)
		return;

	// check if we already have a parser for this project
	if (m_Parsers[project])
		return;

	Manager::Get()->GetMessageManager()->DebugLog(_("Start parsing project %s"), project->GetTitle().c_str());
	int fcount = 0;
	Parser* parser = new Parser(this);
	m_Parsers[project] = parser;
	m_ParsersFilenames[project] = project->GetFilename();
	AddCompilerDirs(parser, project);
	parser->StartTimer();

    if (useCache && ConfigManager::Get()->Read("/code_completion/use_cache", 0L) != 0)
    {
        if (LoadCachedData(parser, project))
            return;
    }

	// parse header files first
	for (int i = 0; i < project->GetFilesCount(); ++i)
	{
		ProjectFile* pf = project->GetFile(i);
		FileType ft = FileTypeOf(pf->relativeFilename);
		if (ft == ftHeader) // only parse header files
		{
            ++fcount;
			parser->Parse(pf->file.GetFullPath());
        }
	}
	// next, parse source files
	for (int i = 0; i < project->GetFilesCount(); ++i)
	{
		ProjectFile* pf = project->GetFile(i);
		FileType ft = FileTypeOf(pf->relativeFilename);
		if (ft == ftSource) // only parse source files
		{
            ++fcount;
			parser->Parse(pf->file.GetFullPath());
        }
	}
	if (fcount == 0)
        Manager::Get()->GetMessageManager()->DebugLog(_("End parsing project %s (no header files found)"), project->GetTitle().c_str());
}

void NativeParser::RemoveParser(cbProject* project, bool useCache)
{
	// check if we already have a parser for this project
	Parser* parser = m_Parsers[project];
	if (!parser)
		return;

    if (useCache && ConfigManager::Get()->Read("/code_completion/use_cache", 0L) != 0)
    {
        if (ConfigManager::Get()->Read("/code_completion/update_cache_always", 0L) != 0 ||
            parser->CacheNeedsUpdate())
        {
            SaveCachedData(parser, m_ParsersFilenames[project]);
        }
    }

	m_Parsers.erase(project);
	m_ParsersFilenames.erase(project);
	if (parser)
		delete parser;
    if (m_pClassBrowser)
		m_pClassBrowser->SetParser(0L);
	Manager::Get()->GetMessageManager()->DebugLog(_("C++ Parser freed"));
}

void NativeParser::AddFileToParser(cbProject* project, const wxString& filename)
{
	Parser* parser = m_Parsers[project];
	if (!parser)
		return;
	parser->Parse(filename, true);
}

void NativeParser::RemoveFileFromParser(cbProject* project, const wxString& filename)
{
	Parser* parser = m_Parsers[project];
	if (!parser)
		return;
	parser->RemoveFile(filename);
}

void NativeParser::ForceReparseActiveProject()
{
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (prj)
    {
        RemoveParser(prj, false);
        AddParser(prj, false);
    }
}

cbProject* NativeParser::FindProjectFromParser(Parser* parser)
{
	for (ParsersMap::iterator it = m_Parsers.begin(); it != m_Parsers.end(); ++it)
	{
		if (parser == it->second)
			return it->first;
	}
	return 0L;
}

cbProject* NativeParser::FindProjectFromEditor(cbEditor* editor)
{
	Parser* parser = FindParserFromEditor(editor);
	return FindProjectFromParser(parser);
}

cbProject* NativeParser::FindProjectFromActiveEditor()
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
    	return 0L;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
	return FindProjectFromEditor(ed);
}

Parser* NativeParser::FindParserFromEditor(cbEditor* editor)
{
    if (!editor)
    	return 0L;

	ProjectFile* pf = editor->GetProjectFile();
	if (!pf)
		return 0L;
	cbProject* project = pf->project;
	return m_Parsers[project];
}	

Parser* NativeParser::FindParserFromActiveEditor()
{
	EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (!edMan)
    	return 0L;
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
	return FindParserFromEditor(ed);
}	

bool NativeParser::LoadCachedData(Parser* parser, cbProject* project)
{
    if (!parser || !project)
        return false;

    wxFileName projectCache = project->GetFilename();
    projectCache.SetExt("cbCache");
    
    wxLogNull ln;
    wxFile f(projectCache.GetFullPath(), wxFile::read);
    if (!f.IsOpened())
        return false;
    
    // read cache file
    Manager::Get()->GetMessageManager()->DebugLog(_("Using parser's existing cache: %s"), projectCache.GetFullPath().c_str());
    bool ret = parser->ReadFromCache(&f);
    DisplayStatus(parser, project);
    return ret;
}

bool NativeParser::SaveCachedData(Parser* parser, const wxString& projectFilename)
{
    if (!parser)
        return false;

    wxFileName projectCache = projectFilename;
    projectCache.SetExt("cbCache");
    
    wxLogNull ln;
    wxFile f(projectCache.GetFullPath(), wxFile::write);
    if (!f.IsOpened())
    {
        Manager::Get()->GetMessageManager()->DebugLog(_("Failed updating parser's cache: %s"), projectCache.GetFullPath().c_str());
        return false;
    }
    
    // write cache file
    Manager::Get()->GetMessageManager()->DebugLog(_("Updating parser's cache: %s"), projectCache.GetFullPath().c_str());
    return parser->WriteToCache(&f);
}

void NativeParser::DisplayStatus(Parser* parser, cbProject* project)
{
    if (!parser || !project)
        return;
    long int tim = parser->GetElapsedTime();
    Manager::Get()->GetMessageManager()->DebugLog(_("Done parsing project %s (%d total parsed files, %d tokens in %d.%d seconds)."),
                    project->GetTitle().c_str(),
                    parser->GetFilesCount(),
                    parser->GetTokens().GetCount(),
                    tim / 1000,
                    tim % 1000);
}

int NativeParser::MarkItemsByAI(bool reallyUseAI)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return 0;

	Parser* parser = FindParserFromActiveEditor();
	if (!parser)
		return 0;

	if (!parser->Done())
		Manager::Get()->GetMessageManager()->DebugLog(_("C++ Parser is still parsing files..."));
	else
	{
		// clear all temporary tokens
		parser->ClearTemporaries();
		bool sort = false;

		// parse function's arguments
		wxString _namespace;
		wxString _procedure;
		if (FindFunctionNamespace(ed, &_namespace, &_procedure))
		{
			Token* token = parser->FindTokenByName(_procedure, false, tkFunction);
			if (token)
			{
				 if (!token->m_Args.IsEmpty() && !token->m_Args.Matches("()"))
				{
					wxString buffer = token->m_Args;
					buffer.Remove(0, 1); // remove (
					buffer.RemoveLast(); // remove )
					buffer.Replace(",", ";"); // replace commas with semi-colons
					buffer << ';'; // aid parser ;)
					Manager::Get()->GetMessageManager()->DebugLog("Parsing arguments: \"%s\"", buffer.c_str());
					if (!parser->ParseBuffer(buffer, false))
						Manager::Get()->GetMessageManager()->DebugLog(_("ERROR parsing block:\n%s"), buffer.c_str());
					sort = true;
				}
			}
		}
		else
			Manager::Get()->GetMessageManager()->DebugLog("Could not find current function's namespace...");
		
		// parse current code block
		int blockStart = FindCurrentBlockStart(ed);
		if (blockStart != -1)
		{
			++blockStart; // skip {
			int blockEnd = ed->GetControl()->GetCurrentPos();
			wxString buffer = ed->GetControl()->GetTextRange(blockStart, blockEnd);
			if (!parser->ParseBuffer(buffer, false))
				Manager::Get()->GetMessageManager()->DebugLog(_("ERROR parsing block:\n%s"), buffer.c_str());
			sort = true;
		}
		else
			Manager::Get()->GetMessageManager()->DebugLog("Could not find current block start...");
		
		if (sort)
			parser->SortAllTokens();

		// clear previously marked tokens
		const TokensArray& tokens = parser->GetTokens();
		for (unsigned int i = 0; i < tokens.GetCount(); ++i)
		{
			Token* token = tokens[i];
			token->m_Bool = !reallyUseAI;
		}

        if (!reallyUseAI)
            return tokens.GetCount();

        // AI will mark (m_Bool == true) every token we should include in list
        return AI(ed, parser);
	}
	return 0;
}

const wxString& NativeParser::GetCodeCompletionItems()
{
    m_CCItems.Clear();

	Parser* parser = FindParserFromActiveEditor();
	if (!parser)
		return m_CCItems;

	int count = MarkItemsByAI();
	if (count)
	{
		const TokensArray& tokens = parser->GetTokens();
		for (unsigned int i = 0; i < tokens.GetCount(); ++i)
		{
			Token* token = tokens[i];
			if (!token->m_Bool)
				continue; // not marked by AI
			token->m_Bool = false; // reset flag for next run
			if (!m_CCItems.IsEmpty())
				 m_CCItems << ";";
			m_CCItems << token->m_Name << token->m_Args;//" " << token->m_Filename << ":" << token->m_Line;
		}
	}
	
	return m_CCItems;
}

const wxArrayString& NativeParser::GetCallTips()
{
    m_CallTips.Clear();

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (!ed)
		return m_CallTips;

	Parser* parser = FindParserFromActiveEditor();
	if (!parser)
		return m_CallTips;

	if (!parser->Done())
		return m_CallTips;

	int line = ed->GetControl()->GetCurrentLine();
	wxString lineText = ed->GetControl()->GetLine(line);
	int end = lineText.Length();
	int nest = 0;
	while (end > 0)
	{
		--end;
		if (lineText.GetChar(end) == ')')
			--nest;
		else if (lineText.GetChar(end) == '(')
		{
			++nest;
			if (nest != 0)
				break;
		}
	}
	if (end == 0)
		return m_CallTips; // no (

	lineText.Remove(end);
	Manager::Get()->GetMessageManager()->DebugLog("Sending \"%s\" for call-tip", lineText.c_str());

	// clear previously marked tokens
	const TokensArray& tokens = parser->GetTokens();
	for (unsigned int i = 0; i < tokens.GetCount(); ++i)
	{
		Token* token = tokens[i];
		token->m_Bool = false;
	}

	// AI will mark (m_Bool == true) every token we should include in list
	if (!AI(ed, parser, lineText, true, true))
		return m_CallTips;
		
	for (unsigned int i = 0; i < tokens.GetCount(); ++i)
	{
		Token* token = tokens[i];
		if (token->m_Bool && !token->m_Args.Matches("()"))
		{
			m_CallTips.Add(token->m_Args);
			token->m_Bool = false; // reset flag for next run
		}
	}
	
	return m_CallTips;
}

// helper funcs

unsigned int NativeParser::FindCCTokenStart(const wxString& line)
{
	int x = line.Length() - 1;
	int nest = 0;

    bool repeat = true;
    while (repeat)
    {
        repeat = false;
        while (x >= 0 && (isalnum(line.GetChar(x)) || line.GetChar(x) == '_'))
            --x;
            
        if (x > 0 &&
            (line.GetChar(x) == '>' && line.GetChar(x - 1) == '-') ||
            (line.GetChar(x) == ':' && line.GetChar(x - 1) == ':'))
        {
            x -= 2;
            repeat = true;
        }
        else if (x >= 0 && line.GetChar(x) == '.')
        {
            --x;
            repeat = true;
        }
        
        if (repeat)
        {
            // check for function/cast ()
            if (x >= 0 && line.GetChar(x) == ')')
            {
                ++nest;
                while (--x >= 0 && nest != 0)
                {
                    switch (line.GetChar(x))
                    {
                        case ')': ++nest; break;
                        case '(': --nest; break;
                    }
                }
                if (x > 0 && (isalnum(line.GetChar(x - 1)) || line.GetChar(x - 1) == '_'))
                    --x;
            }
        }
    }
    ++x;

	if (x < 0)
		x = 0;

    while (line.GetChar(x) == ' ' || line.GetChar(x) == '\t')
        ++x;

	//Manager::Get()->GetMessageManager()->DebugLog("Starting at %d \"%s\"", x, line.Mid(x).c_str());
	return x;
}

wxString NativeParser::GetNextCCToken(const wxString& line, unsigned int& startAt)
{
	wxString res;
	int nest = 0;

    if (startAt < line.Length() && line.GetChar(startAt) == '(')
    {
        while (startAt < line.Length() &&
                (line.GetChar(startAt) == '*' ||
                line.GetChar(startAt) == '&' ||
                line.GetChar(startAt) == '('))
        {
            if (line.GetChar(startAt) == '(')
                ++nest;
            ++startAt;
        }
    }
    
    //Manager::Get()->GetMessageManager()->DebugLog("at %d (%c): res=%s", startAt, line.GetChar(startAt), res.c_str());
    while (startAt < line.Length() && (isalnum(line.GetChar(startAt)) || line.GetChar(startAt) == '_'))
    {
        res << line.GetChar(startAt);
        ++startAt;
    }
    
    while (nest > 0 && startAt < line.Length())
    {
        if (line.GetChar(startAt) == ')')
            --nest;
        ++startAt;
    }
    //Manager::Get()->GetMessageManager()->DebugLog("Done nest: at %d (%c): res=%s", startAt, line.GetChar(startAt), res.c_str());

    if (startAt < line.Length() && line.GetChar(startAt) == '(')
    {
        ++nest;
        while (startAt < line.Length() - 1 && nest != 0)
        {
            ++startAt;
            switch (line.GetChar(startAt))
            {
                case ')': --nest; break;
                case '(': ++nest; break;
            }
        }
        ++startAt;
    }
    
    //Manager::Get()->GetMessageManager()->DebugLog("Return at %d (%c): res=%s", startAt, line.GetChar(startAt), res.c_str());
	return res;
}

wxString NativeParser::GetCCToken(wxString& line, ParserTokenType& tokenType)
{
	// line contains a string on the following form:
	// "    char* mychar = SomeNamespace::m_SomeVar.SomeMeth"
	// first we locate the first non-space char starting from the *end*:
	//
	// "    char* mychar = SomeNamespace::m_SomeVar.SomeMeth"
	//                     ^
	// then we remove everything before it.
	// after it, what we do here, is (by this example) return "SomeNamespace"
	// *and* modify line to become:
	// m_SomeVar.SomeMeth
	// so that if we 're called again with the (modified) line,
	// we 'll return "m_SomeVar" and modify line (again) to become:
	// SomeMeth
	// and so on and so forth until we return an empty string...
	// NOTE: if we find () args in our way, we skip them...

	tokenType = pttSearchText;
	if (line.IsEmpty())
		return wxEmptyString;

	unsigned int x = FindCCTokenStart(line);
	wxString res = GetNextCCToken(line, x);
	//Manager::Get()->GetMessageManager()->DebugLog("FindCCTokenStart returned %d \"%s\"", x, line.c_str());
	//Manager::Get()->GetMessageManager()->DebugLog("GetNextCCToken returned %d \"%s\"", x, res.c_str());
	
	if (x == line.Length())
		line.Clear();
	else
	{
		//Manager::Get()->GetMessageManager()->DebugLog("Left \"%s\"", line.Mid(x).c_str());
		if (line.GetChar(x) == '.')
		{
			tokenType = pttClass;
			line.Remove(0, x + 1);
		}
		else if ((x < line.Length() - 1 && line.GetChar(x) == '-' && line.GetChar(x + 1) == '>') ||
			(x < line.Length() - 1 && line.GetChar(x) == ':' && line.GetChar(x + 1) == ':'))
		{
			if (line.GetChar(x) == ':')
				tokenType = pttNamespace;
			else
				tokenType = pttClass;
			line.Remove(0, x + 2);
		}
		else
			line.Clear();
	}
	return res;
}

int NativeParser::AI(cbEditor* editor, Parser* parser, const wxString& lineText, bool noPartialMatch, bool caseSensitive)
{
	int count = 0;
    int pos = editor->GetControl()->GetCurrentPos();
	m_EditorStartWord = editor->GetControl()->WordStartPosition(pos, true);
	m_EditorEndWord = pos;//editor->GetControl()->WordEndPosition(pos, true);
	int line = editor->GetControl()->GetCurrentLine();

	wxString searchtext;
	//Manager::Get()->GetMessageManager()->DebugLog("********* START **********");

	Token* parentToken = 0L;
	ParserTokenType tokenType;
	wxString actual;
	int col;
	wxString tabwidth;
	tabwidth.Pad(editor->GetControl()->GetTabWidth(), ' ');
	if (lineText.IsEmpty())
	{
		actual = editor->GetControl()->GetLine(line);
		col = editor->GetControl()->GetColumn(pos);
		// replace tabs in line by equal-count spaces because col is in spaces!
		actual.Replace("\t", tabwidth);
		actual.Remove(col);
		actual.Trim();
	}
	else
	{
		actual = lineText;
		col = actual.Length() - 1;
	}
	Manager::Get()->GetMessageManager()->DebugLog("Doing AI for '%s':", actual.c_str());
	
	// find current function's namespace
	wxString procName;
	wxString scopeName;
	FindFunctionNamespace(editor, &scopeName, &procName);
	Token* scopeToken = 0L;
	if (!scopeName.IsEmpty())
		scopeToken = parser->FindTokenByName(scopeName, false, tkNamespace | tkClass);

	while (1)
	{
//		wxString bef = actual;
		wxString tok = GetCCToken(actual, tokenType);
//		wxString aft = actual;
//		Manager::Get()->GetMessageManager()->DebugLog("before='%s', token='%s', after='%s', namespace=%d", bef.c_str(), tok.c_str(), aft.c_str(), tokenType);
		Manager::Get()->GetMessageManager()->DebugLog("tok='%s'", tok.c_str());
		if (tok.IsEmpty())
			break;
		if (actual.IsEmpty() && tokenType == pttSearchText)
		{
			searchtext = tok;
			break;
		}
			
		if (tokenType == pttNamespace)
		{
			//parentToken = parser->FindTokenByName(tok);
			parentToken = parser->FindChildTokenByName(parentToken, tok, true);
			if (!parentToken)
				parentToken = parser->FindChildTokenByName(scopeToken, tok, true); // try local scope
			Manager::Get()->GetMessageManager()->DebugLog("Namespace '%s'", parentToken ? parentToken->m_Name.c_str() : "unknown");
			if (!parentToken)
			{
				Manager::Get()->GetMessageManager()->DebugLog("Bailing out: namespace not found");
				return 0; // fail deliberately
			}
		}
		else
		{
			Token* token = 0L;
			if (tok.Matches("this")) // <-- special case
			{
                token = scopeToken;
                parentToken = scopeToken;
			}
            else
            {
                Manager::Get()->GetMessageManager()->DebugLog("Looking for %s under %s", tok.c_str(), parentToken ? parentToken->m_Name.c_str() : "Unknown");
                token = parser->FindChildTokenByName(parentToken, tok, true);
            }
			if (!token)
			{
                Manager::Get()->GetMessageManager()->DebugLog("Looking for %s under %s", tok.c_str(), scopeToken ? scopeToken->m_Name.c_str() : "Unknown");
				token = parser->FindChildTokenByName(scopeToken, tok, true); // try local scope
            }
			if (token)
                Manager::Get()->GetMessageManager()->DebugLog("Token found %s, type '%s'", token->m_Name.c_str(), token->m_ActualType.c_str());
			if (token && !token->m_ActualType.IsEmpty())
			{
				Manager::Get()->GetMessageManager()->DebugLog("actual type is %s", token->m_ActualType.c_str());
                wxString srch = token->m_ActualType;
				int colon = srch.Find("::");
				if (colon != -1)
				{
                    // type has namespace; break it down and search for it
                    while (!srch.IsEmpty())
                    {
                        parentToken = parser->FindChildTokenByName(parentToken, srch.Left(colon), true);
                        if (!parentToken)
                            break;
                        Manager::Get()->GetMessageManager()->DebugLog("searching under %s", parentToken->m_DisplayName.c_str());
                        srch.Remove(0, colon + 2); // plus two to compensate for "::"
                        colon = srch.Find("::");
                        if (colon == -1)
                            colon = wxSTRING_MAXLEN;
                    }
				}
				else
				{
                    Manager::Get()->GetMessageManager()->DebugLog("Locating %s", token->m_ActualType.c_str());
                    parentToken = parser->FindTokenByName(token->m_ActualType, true, tkClass | tkNamespace | tkEnum);
                    if (!parentToken) // one more, global, try (might be under a namespace)
                        parentToken = parser->FindTokenByName(token->m_ActualType, false, tkClass | tkNamespace | tkEnum);
                }
			}
			Manager::Get()->GetMessageManager()->DebugLog("Class '%s'", parentToken ? parentToken->m_Name.c_str() : "unknown");

			if (!parentToken)
			{
				Manager::Get()->GetMessageManager()->DebugLog("Bailing out: class not found");
				return 0; // fail deliberately
			}
		}
		//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "parentToken=0x%8.8x", parentToken);
	}

    if (noPartialMatch && searchtext.IsEmpty())
        return 0;

	bool bCaseSensitive = parser->Options().caseSensitive || caseSensitive;
	if (!bCaseSensitive)
		searchtext.MakeLower();

	Manager::Get()->GetMessageManager()->DebugLog("Scope='%s'", scopeToken ? scopeToken->m_Name.c_str() : "Unknown");
	if (parentToken)
	{
		Manager::Get()->GetMessageManager()->DebugLog("Final parent: '%s' (count=%d, search=%s)", parentToken->m_Name.c_str(), parentToken->m_Children.GetCount(), searchtext.c_str());
		for (unsigned int i = 0; i < parentToken->m_Children.GetCount(); ++i)
		{
			Token* token = parentToken->m_Children[i];
			if (token->m_IsOperator)
				continue; // ignore operators

			wxString name = token->m_Name;
			if (!bCaseSensitive)
				name.MakeLower();

			bool textCondition;
            if (noPartialMatch)
                textCondition = searchtext.IsEmpty() ? false : name.Matches(searchtext);
            else
            {
                if (lineText.IsEmpty())
                    textCondition = searchtext.IsEmpty() ? true : name.StartsWith(searchtext);
                else
                    textCondition = searchtext.IsEmpty() ? true : name.Matches(searchtext);
            }

            if (token->m_TokenKind == tkEnum)
            {
                // enums children (enumerators), are added by default
                for (unsigned int i2 = 0; i2 < token->m_Children.GetCount(); ++i2)
                {
                    Token* ctok = token->m_Children[i2];
                    ctok->m_Bool = true;
                    ++count;
                }
            }

			// scope-based member access :)
			// display public members
			// private/protected are displayed only if in same scope...
			bool scopeCondition =
						// we can access public members
						token->m_Scope == tsPublic ||
						// we can access private/protected members of current scope only
						scopeToken == parentToken; 

			token->m_Bool = textCondition && scopeCondition;// &&
//							token->m_TokenKind != tkConstructor && // ignore constructors
//							token->m_TokenKind != tkDestructor; // and destructors too
			if (token->m_Bool)
				++count;
		}
		// look for inheritance
		count += DoInheritanceAI(parentToken, scopeToken, searchtext, bCaseSensitive);
	}
	else
	{
		//Manager::Get()->GetMessageManager()->Log(mltDevDebug, "no token");
		// just globals and current function's parent class members here...
		Manager::Get()->GetMessageManager()->DebugLog("Procedure of class '%s' (0x%8.8x), name='%s'", scopeName.c_str(), scopeToken, procName.c_str());
		
		for (unsigned int i = 0; i < parser->GetTokens().GetCount(); ++i)
		{
			Token* token = parser->GetTokens()[i];
			if (token->m_IsOperator)
				continue; // ignore operators

			wxString name = token->m_Name;
			if (!bCaseSensitive)
				name.MakeLower();

			bool textCondition;
			if (!noPartialMatch && lineText.IsEmpty())
				textCondition = searchtext.IsEmpty() ? true : name.StartsWith(searchtext);
			else
				textCondition = searchtext.IsEmpty() ? false : name.Matches(searchtext);

			token->m_Bool = textCondition &&
							(!token->m_pParent || // globals
							token->m_TokenKind == tkEnumerator || // enumerators
							token->m_pParent == scopeToken || // child of procToken
							!lineText.IsEmpty()); // locals too
			if (token->m_Bool)
				++count;
		}
		// look for inheritance
		count += DoInheritanceAI(scopeToken, scopeToken, searchtext, bCaseSensitive);
	}
	return count;
}

int NativeParser::DoInheritanceAI(Token* parentToken, Token* scopeToken, const wxString& searchText, bool caseSensitive)
{
	int count = 0;
	if (!parentToken)
		return 0;
	Manager::Get()->GetMessageManager()->DebugLog("Checking inheritance of %s", parentToken->m_Name.c_str());
	Manager::Get()->GetMessageManager()->DebugLog("- Has %d ancestor(s)", parentToken->m_Ancestors.GetCount());
	for (unsigned int i = 0; i < parentToken->m_Ancestors.GetCount(); ++i)
	{
		Token* ancestor = parentToken->m_Ancestors[i];
		Manager::Get()->GetMessageManager()->DebugLog("- Checking ancestor %s", ancestor->m_Name.c_str());
		int bak = count;
		for (unsigned int x = 0; x < ancestor->m_Children.GetCount(); ++x)
		{
			Token* token = ancestor->m_Children[x];
			wxString name = token->m_Name;
			if (!caseSensitive)
				name.MakeLower();
			bool textCondition = searchText.IsEmpty() ? true : name.StartsWith(searchText);
			//Manager::Get()->GetMessageManager()->DebugLog("- [%s] %s: %s", searchText.c_str(), name.c_str(), textCondition ? "match" : "no match");
			token->m_Bool = textCondition &&
							(token->m_Scope == tsPublic || (scopeToken && scopeToken->InheritsFrom(ancestor)));
			if (token->m_Bool)
				count++;
		}
		Manager::Get()->GetMessageManager()->DebugLog("- %d matches", count - bak);
		count += DoInheritanceAI(ancestor, scopeToken, searchText, caseSensitive);
	}
	return count;
}

bool NativeParser::SkipWhitespaceForward(cbEditor* editor, int& pos)
{
    if (!editor)
        return false;
    wxChar ch = editor->GetControl()->GetCharAt(pos);
    int len = editor->GetControl()->GetLength() - 1;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while (pos < len && (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
        {
            ++pos;
            ch = editor->GetControl()->GetCharAt(pos);
        }
        return true;
    }
    return false;
}

bool NativeParser::SkipWhitespaceBackward(cbEditor* editor, int& pos)
{
    if (!editor)
        return false;
    wxChar ch = editor->GetControl()->GetCharAt(pos);
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while (pos > 0 && (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'))
        {
            --pos;
            ch = editor->GetControl()->GetCharAt(pos);
        }
        return true;
    }
    return false;
}

int NativeParser::FindCurrentBlockStart(cbEditor* editor)
{
	int pos = -1;
	int line = editor->GetControl()->GetCurrentLine();
	while (line >= 0)
	{
		int level = editor->GetControl()->GetFoldLevel(line);
		if ((level & wxSTC_FOLDLEVELHEADERFLAG) &&
			(wxSTC_FOLDLEVELBASE == (level & wxSTC_FOLDLEVELNUMBERMASK)))
		{
			// we 're at block start (maybe '{')
			wxString lineStr = editor->GetControl()->GetLine(line);
			pos = lineStr.Find("{");
			if (pos != wxNOT_FOUND)
			{
				pos += editor->GetControl()->PositionFromLine(line);
				break;
			}
		}
		--line;
	}
	return pos;
}

bool NativeParser::FindFunctionNamespace(cbEditor* editor, wxString* nameSpace, wxString* procName)
{
	if (!nameSpace && !procName)
		return false;

	int posOf = FindCurrentBlockStart(editor);
	if (posOf != wxNOT_FOUND)
	{
		// look for :: right before procname and procargs
		// if we find a }, we failed (probably no class member this function)...
		bool done = false;
		int nest = 0;
		bool passedArgs = false;
		bool hasNS = false;
		while (posOf > 0)
		{	
			--posOf;
			char ch = editor->GetControl()->GetCharAt(posOf);
			switch (ch)
			{
				case ')' : --nest; passedArgs = false; break;
                case '(' :
                    ++nest;
                    passedArgs = nest == 0;
                    if (passedArgs)
                    {
                        --posOf;
                        SkipWhitespaceBackward(editor, posOf);
                    }
                    break;
			}
			if (passedArgs)
			{
                if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == ':') 
                {
                    int bkp = posOf;
                    SkipWhitespaceBackward(editor, posOf);
                    done = true;
                    hasNS = ch == ':' && editor->GetControl()->GetCharAt(posOf - 1) == ':';
                    posOf = bkp;
                }
            }
			if (done || ch == '}' || ch == ';')
				break;
		}
        Manager::Get()->GetMessageManager()->DebugLog("Pos=%d", posOf);
		if (done)
		{
			if (procName)
			{
				int procEnd = editor->GetControl()->WordEndPosition(posOf + 1, true);
				*procName = editor->GetControl()->GetTextRange(posOf + 1, procEnd);
			}
			if (nameSpace && hasNS)
			{
                nameSpace->Clear();
				posOf -= 2;
				int scopeStart = editor->GetControl()->WordStartPosition(posOf, true);
				*nameSpace = editor->GetControl()->GetTextRange(scopeStart, posOf + 1);
			}
            Manager::Get()->GetMessageManager()->DebugLog("NS: '%s', PROC: '%s'", nameSpace ? nameSpace->c_str() : "", procName ? procName->c_str() : "");
			return true;
		}
	}
	else
        Manager::Get()->GetMessageManager()->DebugLog("Can't find block start.");
	return false;
}

// events

void NativeParser::OnThreadStart(wxCommandEvent& event)
{
//	 nothing for now
}

void NativeParser::OnThreadEnd(wxCommandEvent& event)
{
//	 nothing for now
}


void NativeParser::OnParserEnd(wxCommandEvent& event)
{
	Parser* parser = (Parser*)event.GetInt();
	if (parser && parser->Done())
	{
		cbProject* project = FindProjectFromParser(parser);
		if (project)
            DisplayStatus(parser, project);
		else
			Manager::Get()->GetMessageManager()->DebugLog(_("Parser aborted (project closed)."));

		if (project == Manager::Get()->GetProjectManager()->GetActiveProject())
		{
            Manager::Get()->GetMessageManager()->DebugLog(_("Updating class browser..."));
			if (m_pClassBrowser)
			{
				m_pClassBrowser->SetParser(parser);
				m_pClassBrowser->Update();
			}
            Manager::Get()->GetMessageManager()->DebugLog(_("Class browser updated."));
		}
	}
}