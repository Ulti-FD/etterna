#include "global.h"
#include "ScreenInstallOverlay.h"
#include "RageFileManager.h"
#include "ScreenManager.h"
#include "Preference.h"
#include "RageLog.h"
#include "json/value.h"
#include "JsonUtil.h"
#include "SpecialFiles.h"
class Song;
#include "SongManager.h"
#include "GameState.h"
#include "GameManager.h"
#include "ScreenSelectMusic.h"
#include "CommonMetrics.h"
#include "SongManager.h"
#include "CommandLineActions.h"
#include "ScreenDimensions.h"
#include "StepMania.h"
#include "ActorUtil.h"

#if !defined(WITHOUT_NETWORKING)
#include "DownloadManager.h"
#endif


struct FileCopyResult
{
	FileCopyResult(RString _sFile, RString _sComment) : sFile(_sFile), sComment(_sComment) {}
	RString sFile, sComment;
};

static bool IsStepManiaProtocol(const RString &arg)
{
	// for now, only load from the StepMania domain until the security implications of this feature are better understood.
	//return BeginsWith(arg,"stepmania://beta.stepmania.com/");
	return BeginsWith(arg, "http://") || BeginsWith(arg, "https://");
}

static bool IsPackageFile(const RString &arg)
{
	RString ext = GetExtension(arg);
	return ext.EqualsNoCase("smzip") || ext.EqualsNoCase("zip");
}

void DoInstalls(CommandLineActions::CommandLineArgs args)
{
	bool reload = false;
	for (int i = 0; i<(int)args.argv.size(); i++)
	{
		RString s = args.argv[i];
		if (IsStepManiaProtocol(s))
		{

#if !defined(WITHOUT_NETWORKING)
			DLMAN->Download(s);
			//g_pDownloadTasks.push_back(new DownloadTask(s));
#else
			// TODO: Figure out a meaningful log message.
#endif
		}
		else if (IsPackageFile(s)) {
			InstallSmzipOsArg(s);
			reload = true;
		}
	}
	if (reload) {
		auto screen = SCREENMAN->GetScreen(0);
		if (screen && screen->GetName() == "ScreenSelectMusic") {
			static_cast<ScreenSelectMusic*>(screen)->DifferentialReload();
		}
		else
		{
			SONGMAN->DifferentialReload();
		}
	}
	return;
}

REGISTER_SCREEN_CLASS(ScreenInstallOverlay);

ScreenInstallOverlay::~ScreenInstallOverlay()
= default;
void ScreenInstallOverlay::Init()
{
	Screen::Init();

	m_textStatus.LoadFromFont(THEME->GetPathF("ScreenInstallOverlay", "status"));
	m_textStatus.SetName("Status");
	LOAD_ALL_COMMANDS_AND_SET_XY_AND_ON_COMMAND(m_textStatus);


	ActorUtil::LoadAllCommandsAndSetXY(m_textStatus, "ScreenInstallOverlay");
	this->AddChild(&m_textStatus);
	curl_global_init(CURL_GLOBAL_ALL);
}

bool ScreenInstallOverlay::Input(const InputEventPlus &input)
{
	/*
	if( input.DeviceI.button == g_buttonLogin && input.type == IET_FIRST_PRESS )
	{
	HOOKS->GoToURL("http://www.stepmania.com/launch.php");
	return true;
	}
	*/

	return Screen::Input(input);
}

void ScreenInstallOverlay::Update(float fDeltaTime)
{
	Screen::Update(fDeltaTime);
	while (CommandLineActions::ToProcess.size() > 0)
	{
		CommandLineActions::CommandLineArgs args = CommandLineActions::ToProcess.back();
		CommandLineActions::ToProcess.pop_back();
		DoInstalls(args);
	}
#if !defined(WITHOUT_NETWORKING)
	DLMAN->UpdateAndIsFinished(fDeltaTime);
	vector<download*>& dls = DLMAN->downloads;
	vector<RString> vsMessages;
	for(auto dl : dls)
	{
		vsMessages.push_back(dl->status);
	}
	m_textStatus.SetText(join("\n", vsMessages));

#endif
}

/*
* (c) 2001-2005 Chris Danford, Glenn Maynard
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, and/or sell copies of the Software, and to permit persons to
* whom the Software is furnished to do so, provided that the above
* copyright notice(s) and this permission notice appear in all copies of
* the Software and that both the above copyright notice(s) and this
* permission notice appear in supporting documentation.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
* THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
* INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
* OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
* OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
* OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*/
