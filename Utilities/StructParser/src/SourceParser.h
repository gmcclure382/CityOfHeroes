#ifndef _SOURCEPARSER_H_
#define _SOURCEPARSER_H_

#include "tokenizer.h"

#include "FileListLoader.h"
#include "FileListWriter.h"

#include "IdentifierDictionary.h"
#include "SourceParserBaseClass.h"

#include "utils.h"

#include <vector>
#include <string>
#include <filesystem>

#define MAX_BASE_SOURCE_PARSERS 8

#define MAX_MAGIC_WORDS_PER_BASE_SOURCE_PARSER 12

#define MAX_WIKI_PROJECTS 64
#define MAX_WIKI_CATEGORIES 256

typedef class AutoRunManager AutoRunManager;



class SourceParser
{
public:
    SourceParser();
    ~SourceParser();

	/*
	 * Generate parser data for one target on a fresh SourceParser instance.
	 * All arguments are borrowed inputs. targetName and configuration must
	 * be nonempty; directory and source paths must be absolute. srcDir,
	 * commonDir and projectDir must exist. isExecutable selects executable
	 * autorun output. sourceFiles is the complete list supplied via stdin;
	 * only eligible C sources and headers are scanned.
	 *
	 * Reads source files, parser settings and cached dependencies; creates
	 * or updates AutoGen files and intermediate build markers. Returns 0
	 * on success, including an unchanged-input quick exit. Filesystem
	 * errors throw; parser assertion failures terminate the process.
	 * Uses global parser/file-wrapper state and is not thread-safe.
	 */
	int ParseSource(
		std::string const& targetName,
		std::filesystem::path const& srcDir,
		std::filesystem::path const& commonDir,
		std::filesystem::path const& outDir,
		std::filesystem::path const& intDir,
		std::string const& configuration,
		std::filesystem::path const& projectDir,
		bool isExecutable,
		std::vector<std::filesystem::path> const& sourceFiles);

    void NukeCObjFile(char *pFileName);

    char const* GetShortProjectName() { return m_shortenedPrjFileName.c_str(); }
    char const* GetSoureDir() { return m_srcDir.c_str(); }
    IdentifierDictionary *GetDictionary() { return &m_IdentifierDictionary; }

    void SetExtraDataFlagForFile(char *pFileName, int iFlag);

    AutoRunManager *GetAutoRunManager() { return m_pAutoRunManager; }

    //returns true if the project is the game client, or a lib that is linked only into the game client
    bool ProjectIsClientOrClientOnlyLib(void);

    //returns true if the projet is the game server, or a lib that is linked only into the game server
    bool ProjectIsGameServerOrGameServerOnlyLib(void);

    //returns true if the project is an executable as opposed to a library
    bool ProjectIsExecutable(void) { return m_bIsAnExecutable; }

    int GetNumProjectFiles(void) { return m_iNumProjectFiles; }
    char *GetNthProjectFile(int n) { return m_ProjectFiles[n]; }

    bool DoesVariableHaveValue(char const* pVarName, char const* pValue, bool bCheckFinalValueOnly);


private://structs
    typedef struct SourceParserVar
    {
        char *pVarName;
        char *pValue;
        struct SourceParserVar *pNext;
    } SourceParserVar;


private:
    FileListLoader *m_pFileListLoader;
    FileListWriter *m_pFileListWriter;

    IdentifierDictionary m_IdentifierDictionary;

    int m_iNumSourceParsers;
    SourceParserBaseClass *m_pSourceParsers[MAX_BASE_SOURCE_PARSERS];
    AutoRunManager *m_pAutoRunManager;

    int m_iNumProjectFiles;
    char m_ProjectFiles[MAX_FILES_IN_PROJECT][MAX_PATH];
    bool m_bFilesNeedToBeUpdated[MAX_FILES_IN_PROJECT];

    int m_iNumDependencies[MAX_FILES_IN_PROJECT];
    int m_iDependencies[MAX_FILES_IN_PROJECT][MAX_DEPENDENCIES_SINGLE_FILE];

    int m_iExtraDataPerFile[MAX_FILES_IN_PROJECT];

    std::string m_shortenedPrjFileName;

    std::string m_intDir;
    std::string m_outDir;
    std::string m_srcDir;
    std::string m_commonDir;
    std::string m_prjDir;

    //esnure that the project contains the two master autogen files
    bool m_FoundAutoGenFile1;
    bool m_FoundAutoGenFile2;

    char m_AutoGenFile1Name[MAX_PATH];
    char m_AutoGenFile2Name[MAX_PATH];

    char m_SpecialAutoRunFuncName[MAX_PATH];

    //whether the project we're working on is an executable vs. a library
    bool m_bIsAnExecutable;

    std::string m_configuration;

    SourceParserVar *m_pFirstVar;

private:
	void AddProjectFiles(
		std::vector<std::filesystem::path> const& files);
    bool NeedToUpdateFile(char *pFileName, int iExtraData,  bool bForceUpdateUnlessFileDoesntExist);
    void ScanSourceFile(char *pSourceFile);
    
    void LoadSavedDependenciesAndRemoveObsoleteFiles(void);

    //returns true if at least one file was set to udpate that was previously not set to update
    //
    //find all need-to-update files which have dependencies, and set all the other
    //files they are dependent on to be need-to-update, and recurse. 
    bool ProcessAllLoadedDependencies();
    void ClearAllDependenciesForUpdatingFiles(void);
    void AddDependency(int iFile1, int iFile2);
    void ProcessAllFiles_ReadAll();
    void ProcessAllFiles();
    int FindProjectFileIndex(char *pFileName);
    void DestroyLegacyMasterFiles(bool bBuildAll);
    void MakeAutoGenDirectory();
    void CheckForRequiredFiles(const char *pFileName);
    bool DidCleanBuildJustHappen();
    void CleanOutAllAutoGenFiles();
    bool IsQuickExitPossible();
	bool SavedFileListMatchesCurrentProjectFiles();
    void CreateCleanBuildMarkerFile();
    void CreateParsers(void);
    bool MakeSpecialAutoRunFunction(void);

    void AddVariableValue(char *pVarName, char *pValue);
    void SetVariablesFromTokenizer(Tokenizer *pTokenizer, char *pStartingDirectory);
    void FindVariablesFileAndLoadVariables(void);
};


//global TRACE for verbose stuff
extern int gVerbose;
#define TRACE(...)  {if (gVerbose) {printf(__VA_ARGS__); fflush(stdout);}}

#define GENERATE_FAKE_DEPENDENCIES 1
#endif
