// Scintilla source code edit control
/** @file LexSQL.cxx
 ** Lexer for SQL.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "Platform.h"

#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"
#include "Scintilla.h"
#include "SciLexer.h"

static void classifyWordSQL(unsigned int start, unsigned int end, WordList *keywordlists[], Accessor &styler) {
	char s[100];
	bool wordIsNumber = isdigit(styler[start]) || (styler[start] == '.');
	for (unsigned int i = 0; i < end - start + 1 && i < 30; i++) {
		s[i] = static_cast<char>(tolower(styler[start + i]));
		s[i + 1] = '\0';
	}

	WordList &keywords1 = *keywordlists[0];
	WordList &keywords2 = *keywordlists[1];

	char chAttr = SCE_C_IDENTIFIER;
	if (wordIsNumber)
		chAttr = SCE_C_NUMBER;
	else {
		if (keywords1.InList(s)) {
			chAttr = SCE_C_WORD;
		}

		if (keywords2.InList(s)) {
			chAttr = SCE_C_WORD2;
		}
	}
	styler.ColourTo(end, chAttr);
}

static void ColouriseSQLDoc(unsigned int startPos, int length,
                            int initStyle, WordList *keywordlists[], Accessor &styler) {

	styler.StartAt(startPos);

	bool fold = styler.GetPropertyInt("fold") != 0;
	bool sqlBackslashEscapes = styler.GetPropertyInt("sql.backslash.escapes", 0) != 0;
	int lineCurrent = styler.GetLine(startPos);
	int spaceFlags = 0;

	int state = initStyle;
	char chPrev = ' ';
	char chNext = styler[startPos];
	styler.StartSegment(startPos);
	unsigned int lengthDoc = startPos + length;
	for (unsigned int i = startPos; i < lengthDoc; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags);
			int lev = indentCurrent;
			if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
				// Only non whitespace lines can be headers
				int indentNext = styler.IndentAmount(lineCurrent + 1, &spaceFlags);
				if (indentCurrent < (indentNext & ~SC_FOLDLEVELWHITEFLAG)) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				}
			}
			if (fold) {
				styler.SetLevel(lineCurrent, lev);
			}
		}

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			chPrev = ' ';
			i += 1;
			continue;
		}

		if (state == SCE_C_DEFAULT) {
			if (iswordstart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_WORD;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENT;
			} else if (ch == '-' && chNext == '-') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINEDOC;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_CHARACTER;
			} else if (ch == '"') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_STRING;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_C_OPERATOR);
			}
		} else if (state == SCE_C_WORD) {
			if (!iswordchar(ch)) {
				classifyWordSQL(styler.GetStartSegment(), i - 1, keywordlists, styler);
				state = SCE_C_DEFAULT;
				if (ch == '/' && chNext == '*') {
					state = SCE_C_COMMENT;
				} else if (ch == '-' && chNext == '-') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '#') {
					state = SCE_C_COMMENTLINEDOC;
				} else if (ch == '\'') {
					state = SCE_C_CHARACTER;
				} else if (ch == '"') {
					state = SCE_C_STRING;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_C_OPERATOR);
				}
			}
		} else {
			if (state == SCE_C_COMMENT) {
				if (ch == '/' && chPrev == '*') {
					if (((i > (styler.GetStartSegment() + 2)) || ((initStyle == SCE_C_COMMENT) &&
					    (styler.GetStartSegment() == startPos)))) {
						styler.ColourTo(i, state);
						state = SCE_C_DEFAULT;
					}
				}
			} else if (state == SCE_C_COMMENTLINE || state == SCE_C_COMMENTLINEDOC) {
				if (ch == '\r' || ch == '\n') {
					styler.ColourTo(i - 1, state);
					state = SCE_C_DEFAULT;
				}
			} else if (state == SCE_C_CHARACTER) {
				if (sqlBackslashEscapes && ch == '\\') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				} else if (ch == '\'') {
					if (chNext == '\'') {
						i++;
					} else {
						styler.ColourTo(i, state);
						state = SCE_C_DEFAULT;
						i++;
					}
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (state == SCE_C_STRING) {
				if (ch == '"') {
					if (chNext == '"') {
						i++;
					} else {
						styler.ColourTo(i, state);
						state = SCE_C_DEFAULT;
						i++;
					}
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			}
			if (state == SCE_C_DEFAULT) {    // One of the above succeeded
				if (ch == '/' && chNext == '*') {
					state = SCE_C_COMMENT;
				} else if (ch == '-' && chNext == '-') {
					state = SCE_C_COMMENTLINE;
				} else if (ch == '#') {
					state = SCE_C_COMMENTLINEDOC;
				} else if (ch == '\'') {
					state = SCE_C_CHARACTER;
				} else if (ch == '"') {
					state = SCE_C_STRING;
				} else if (iswordstart(ch)) {
					state = SCE_C_WORD;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_C_OPERATOR);
				}
			}
		}
		chPrev = ch;
	}
	styler.ColourTo(lengthDoc - 1, state);
}

static const char * const sqlWordListDesc[] = {
	"Keywords",
	"Database Objects",
	0
};

LexerModule lmSQL(SCLEX_SQL, ColouriseSQLDoc, "sql", 0, sqlWordListDesc);
