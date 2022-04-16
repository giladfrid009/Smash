#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H

enum class Commands
{
	Unknown,
	Special,
	ChangePrompt,
	ShowPid,
	PrintDir,
	ChangeDir,
	Jobs,
	Kill,
	Foreground,
	Background,
	Quit,
	Tail,
	Touch,
};

enum class SpecialCommands
{
	None,
	PipeOut,
	PipeErr,
	RedirectWrite,
	RedirectAppend,
	Timeout
};

class Identifiers
{
	public:

	const char* PipeOut = "|";
	const char* PipeErr = "|&";
	const char* RedirectWrite = ">";
	const char* RedirectAppend = ">>";
	const char* Timeout = "timeout";
	const char* ChangePrompt = "chprompt";
	const char* ShowPid = "showpid";
	const char* PrintDir = "pwd";
	const char* ChangeDir = "cd";
	const char* Jobs = "jobs";
	const char* Kill = "kill";
	const char* Foreground = "fg";
	const char* Background = "bg";
	const char* Quit = "quit";
	const char* Tail = "tail";
	const char* Touch = "touch";
};

#endif // ! IDENTIFIERS_H
