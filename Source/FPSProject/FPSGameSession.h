

#pragma once

#include "Online.h"
#include "GameFramework/GameSession.h"
#include "FPSGameSession.generated.h"

/**
 * 
 */

struct FGameSessionParams
{
	FName SessionName;
	bool bIsLAN;
	bool bIsPresence;
	int32 ControllerId;
	int32 BestSessionIdx;

	FGameSessionParams()
		: SessionName(NAME_None)
		, bIsLAN(false)
		, bIsPresence(false)
		, ControllerId(0)
		, BestSessionIdx(0)
	{
	}
};

UCLASS(config=Game)
class FPSPROJECT_API AFPSGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()

protected:

	/** DELEGATES */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	FGameSessionParams CurrentSessionParams;
	/** Host Settings */
	TSharedPtr<class FFPSOnlineGameSettings> HostSettings;
	/** Search Settings */
	TSharedPtr<class FFPSOnlineSearchSettings> SearchSettings;

	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, bool bWasSuccessful);

	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void DelayedSessionDelete();

	void ChooseBestSession();

	DECLARE_EVENT_TwoParams(AFPSGameSession, FOnCreatePresenceSessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);
	FOnCreatePresenceSessionComplete CreatePresenceSessionCompleteEvent;

	/** The even that is triggered when joining a session */
	DECLARE_EVENT_OneParam(AFPSGameSession, FOnJoinSessionComplete, bool /*bWasSuccessful*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	DECLARE_EVENT_OneParam(AShooterGameSession, FOnFindSessionsComplete, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

	void FindSessions(int32 ControllerId, FName SessionName, bool bIsLAN, bool bIsPresence);

	bool JoinSession(int32 ControllerId, FName Sessionname, int32 SessionIndexInSearchResults);

	bool IsBusy() const;

	/** Create the game session*/

	virtual void CreateGameSession(int32 ControllerId);

	/**Override: Control of starting the match */
	virtual void HandleMatchHasStarted() override;
	
	/**Override: Control of finishing the match */
	virtual void HandleMatchHasEnded() override;

	/**Destroy the game session */

	virtual void DestroyGameSession(int32 ControllerId);

	/** Used to travel to the specific URL of the destination session */
	bool TravelToSession(int32 ControllerId, FName SessionName);

	

public:
	static const int32 DEFAULT_NUM_PLAYERS = 8;


	bool HostSession(int32 ControllerId, FName SessionName, const FString & GameType, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);


	/** @return the delegate fired when creating a presence session */
	FOnCreatePresenceSessionComplete& OnCreatePresenceSessionComplete() { return CreatePresenceSessionCompleteEvent; }
	/** Returns the deleate fired when joining a session */
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }

	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }
	
};
