#pragma once
#include "FPSProject.h"
#include "Online.h"

class FFPSOnlineGameSettings : public FOnlineSessionSettings
{
public:
	FFPSOnlineGameSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 8);
	virtual ~FFPSOnlineGameSettings() {}
};

class FFPSOnlineSearchSettings : public FOnlineSessionSearch
{

public:
	FFPSOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FFPSOnlineSearchSettings() {}
};

class FFPSOnlineSearchSettingsEmptyDedicated : public FFPSOnlineSearchSettings
{
public:
	FFPSOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);
	virtual ~FFPSOnlineSearchSettingsEmptyDedicated() {}
};