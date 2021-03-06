#ifndef COMPILER_H
#define COMPILER_H

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/dynarray.h>
#include "compileoptionsbase.h"
#include "compileroptions.h"

/*
    Macros used in command specs:
    
    Compiler executable: $compiler
    Resource compiler executable: $rescomp
    Linker executable: $linker
    Linker executable for static libs: $lib_linker
    Compiler options: $options
    Linker options: $link_options
    Include dirs: $includes
    Rsource include dirs: $res_includes
    Library dirs: $libdirs
    Link libraries: $libs
    Source file: $file
    Object file: $object
    Dependency result: $dep_object
    All object files: $objects
    All *linkable* object files: $link_objects
    Exe output file: $exe_output
    Static lib output file: $static_output
    Dynamic lib output file: $dynamic_output
    Dynamic lib DEF output file: $def_output
    Resources output file: $resource_output
    
    Usual special chars apply: \t, \n, etc.
    The command output should be ready for inclusion
    in a Makefile (properly tab aligned but *no* tabs at the start of the string)
    The macro substitution happens in compiler's MakefileGenerator.
*/

/// Enum categorizing compiler's output line as warning/error/normal
enum CompilerLineType
{
    cltNormal,
    cltWarning,
    cltError
};

// regexes array declaration
struct RegExStruct
{
    RegExStruct()
        : desc(_("Unknown")), lt(cltError), regex(_T("")), filename(0), line(0)
    {
        memset(msg, 0, sizeof(msg));
    }
    RegExStruct(const RegExStruct& rhs)
        : desc(rhs.desc), lt(rhs.lt), regex(rhs.regex), filename(rhs.filename), line(rhs.line)
    {
        memcpy(msg, rhs.msg, sizeof(msg));
    }
    RegExStruct(const wxString& _desc,
                CompilerLineType _lt,
                const wxString& _regex,
                int _msg,
                int _filename = 0,
                int _line = 0,
                int _msg2 = 0,
                int _msg3 = 0)
        : desc(_desc), lt(_lt), regex(_regex), filename(_filename), line(_line)
    {
        msg[0] = _msg;
        msg[1] = _msg2;
        msg[2] = _msg3;
    }
    wxString desc; // title of this regex
    CompilerLineType lt; // classify the line, if regex matches
    wxString regex; // the regex to match
    int msg[3]; // up-to 3 sub-expression nr for warning/error message
    int filename; // sub-expression nr for filename
    int line; // sub-expression nr for line number
    // if more than one sub-expressions are entered for msg,
    // they are appended to each other, with one space in between.
    // Appending takes place in the same order...
};
WX_DECLARE_OBJARRAY(RegExStruct, RegExArray);

/// Helper enum to retrieve compiler commands
enum CommandType
{
    ctCompileObjectCmd = 0, ///< Compile object command, e.g. "$compiler $options $includes -c $file -o $object"
    ctGenDependenciesCmd,   ///< Generate dependencies command
    ctCompileResourceCmd,   ///< Compile Win32 resources command, e.g. "$rescomp -i $file -J rc -o $resource_output -O coff $includes"
    ctLinkExeCmd,           ///< Link executable command, e.g. "$linker $libdirs -o $exe_output $link_objects $libs -mwindows"
    ctLinkConsoleExeCmd,    ///< Link console executable command, e.g. "$linker $libdirs -o $exe_output $link_objects $libs"
    ctLinkDynamicCmd,       ///< Link dynamic (dll) lib command, e.g. "$linker -shared -Wl,--output-def=$def_output -Wl,--out-implib=$static_output -Wl,--dll $libdirs $link_objects $libs -o $dynamic_output"
    ctLinkStaticCmd         ///< Link static lib command, e.g. "ar -r $output $link_objects\n\tranlib $static_output"
};
#define COMPILER_COMMAND_TYPES_COUNT 7 // change this to reflect the above enumerators count

/// Helper enum for type of compiler logging
enum CompilerLoggingType
{
    clogFull,
    clogSimple,
    clogNone
};

// Helper enum for compiler build method
enum CompilerBuildMethod
{
    cbmUseMake,
    cbmDirect
};

enum AutoDetectResult
{
    adrDetected,
    adrGuessed
};

/// Struct to keep programs
struct CompilerPrograms
{
    wxString C; // C compiler
    wxString CPP; // C++ compiler
    wxString LD; // dynamic libs linker
    wxString LIB; // static libs linker
    wxString WINDRES; // resource compiler
    wxString MAKE; // make
    wxString DBG; // debugger
};

/// Struct to keep switches
struct CompilerSwitches
{
    wxString includeDirs;   // -I
    wxString libDirs;       // -L
    wxString linkLibs;      // -l
    wxString defines;       // -D
    wxString genericSwitch; // -
    wxString objectExtension; // o
    bool forceLinkerUseQuotes; // use quotes for filenames in linker command line (needed or not)?
    bool forceCompilerUseQuotes; // use quotes for filenames in compiler command line (needed or not)?
    bool needDependencies; // true
    CompilerLoggingType logging; // clogFull
    CompilerBuildMethod buildMethod; // cbmDirect
    wxString libPrefix; // lib
    wxString libExtension; // a
    bool linkerNeedsLibPrefix; // when adding a link library, linker needs prefix?
    bool linkerNeedsLibExtension; // when adding a link library, linker needs extension?
};

/**
  * @brief Abstract base class for compilers.
  *
  * Create a derived class and add it in compilerfactory.cpp
  */
class DLLIMPORT Compiler : public CompileOptionsBase
{
	public:
		Compiler(const wxString& name);
		Compiler(const Compiler& other); // copy ctor to copy everything but update m_ID
		virtual ~Compiler();
		
		/** @brief Check if the supplied string is a compiler warning/error */
		virtual CompilerLineType CheckForWarningsAndErrors(const wxString& line);
		/** @brief Returns warning/error filename. Use it after a call to CheckForWarningsAndErrors() */
		virtual wxString GetLastErrorFilename(){ return m_ErrorFilename; }
		/** @brief Returns warning/error line number (as a string). Use it after a call to CheckForWarningsAndErrors() */
		virtual wxString GetLastErrorLine(){ return m_ErrorLine; }
		/** @brief Returns warning/error actual string. Use it after a call to CheckForWarningsAndErrors() */
		virtual wxString GetLastError(){ return m_Error; }
        /** @brief Get the compiler's name */
		virtual const wxString& GetName() const { return m_Name; }
        /** @brief Get the compiler's master path (must contain "bin", "include" and "lib") */
		virtual const wxString& GetMasterPath() const { return m_MasterPath; }
        /** @brief Get the compiler's extra paths */
		virtual const wxArrayString& GetExtraPaths() const { return m_ExtraPaths; }
        /** @brief Get the compiler's programs */
		virtual const CompilerPrograms& GetPrograms() const { return m_Programs; }
        /** @brief Get the compiler's generic switches */
		virtual const CompilerSwitches& GetSwitches() const { return m_Switches; }
        /** @brief Get the compiler's options */
		virtual const CompilerOptions& GetOptions() const { return m_Options; }
        /** @brief Get a command based on CommandType */
		virtual const wxString& GetCommand(CommandType ct) const { return m_Commands[(int)ct]; }
		/** @brief Get the array of regexes used in errors/warnings recognition */
		virtual const RegExArray& GetRegExArray(){ return m_RegExes; }
		/** @brief Load the default (preset) array of regexes used in errors/warnings recognition */
		virtual void LoadDefaultRegExArray() = 0;

        /** @brief Set the compiler's name */
		virtual void SetName(const wxString& name){ m_Name = name; }
        /** @brief Set the compiler's master path (must contain "bin", "include" and "lib") */
		virtual void SetMasterPath(const wxString& path){ m_MasterPath = path; }
        /** @brief Set the compiler's extra paths */
		virtual void SetExtraPaths(const wxArrayString& paths){ m_ExtraPaths = paths; }
        /** @brief Set the compiler's programs */
		virtual void SetPrograms(const CompilerPrograms& programs){ m_Programs = programs; }
        /** @brief Set the compiler's generic switches */
		virtual void SetSwitches(const CompilerSwitches& switches){ m_Switches = switches; }
        /** @brief Set the compiler's options */
		virtual void SetOptions(const CompilerOptions& options){ m_Options = options; }
        /** @brief Set a command based on CommandType */
		virtual void SetCommand(CommandType ct, const wxString& cmd){ m_Commands[(int)ct] = cmd; }
		/** @brief Set the array of regexes used in errors/warnings recognition */
		virtual void SetRegExArray(const RegExArray& regexes){ m_RegExes = regexes; }

        /** @brief Save settings */
        virtual void SaveSettings(const wxString& baseKey);
        /** @brief Load settings */
        virtual void LoadSettings(const wxString& baseKey);
        /** @brief Reset settings to defaults.
          * Put initialization code here and call this from the default constructor.
          */
        virtual void Reset() = 0;
        /** @brief Try to auto-detect the compiler's installation directory */
        virtual AutoDetectResult AutoDetectInstallationDir() = 0;

        /** @brief Get this compiler's unique ID */
        long GetID() const { return m_ID; }
        /** @brief Get this compiler's parent's unique ID */
        long GetParentID() const { return m_ParentID; }

        /** @brief Get the command type descriptions (used in advanced compiler options) */
        static wxString CommandTypeDescriptions[COMPILER_COMMAND_TYPES_COUNT];
	protected:
        friend class CompilerFactory;
        /** @brief Implement this in new compilers, to return a new copy */
        virtual Compiler* CreateCopy() = 0;
        // set the following members in your class
        wxString m_Name;
        wxString m_MasterPath;
        wxArrayString m_ExtraPaths;
        wxString m_Commands[COMPILER_COMMAND_TYPES_COUNT];
        CompilerPrograms m_Programs;
        CompilerSwitches m_Switches;
        CompilerOptions m_Options;
        RegExArray m_RegExes;
        wxString m_ErrorFilename;
        wxString m_ErrorLine;
        wxString m_Error;
	private:
        long m_ID;
        long m_ParentID; // -1 for builtin compilers, the builtin compiler's ID to derive from for user compilers...
        static long CompilerIDCounter; // for builtin compilers
        static long UserCompilerIDCounter; // for user compilers
};

#endif // COMPILER_H
