#pragma once
#include "Defragged.h"
#include "Online.h"

class FDFOnlineGameSettings : public FOnlineSessionSettings
{
public:
	FDFOnlineGameSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 8);
	virtual ~FDFOnlineGameSettings() {}
};

class FDFOnlineSearchSettings : public FOnlineSessionSearch
{

public:
	FDFOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FDFOnlineSearchSettings() {}
};

class FDFOnlineSearchSettingsEmptyDedicated : public FDFOnlineSearchSettings
{
public:
	FDFOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);
	virtual ~FDFOnlineSearchSettingsEmptyDedicated() {}
};