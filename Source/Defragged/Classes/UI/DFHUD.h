

#pragma once

#include "GameFramework/HUD.h"
#include "DFHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEFRAGGED_API ADFHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
    
    //Primary draw call for the HUD
	virtual void DrawHUD() override;

protected:
	/** Floor for automatic hud scaling. */
	static const float MinHudScale;

    //Crosshair asset pointer
	UPROPERTY()
    UTexture2D* CrosshairTex;
	


	//Health Bar
	UPROPERTY()
		UTexture2D* Health;

	//Health bar container
	UPROPERTY()
		UTexture2D* HealthContainer;

	//Right Devide
	UPROPERTY()
		UTexture2D* RightDevide;

	//Right Wing Thing
	UPROPERTY()
		UTexture2D* RightWing;

	//Left Wing Thing
	UPROPERTY()
		UTexture2D* LeftWing;

	//Virus Score Bar
	UPROPERTY()
		UTexture2D* ScoreBarVirus;

	//Anti-Virus Score Bar
	UPROPERTY()
		UTexture2D* ScoreBarAnti;

	//Score Container Bottem
	UPROPERTY()
		UTexture2D* ScoreBarContainerBot;

	//Score Container top
	UPROPERTY()
		UTexture2D* ScoreBarContainerTop;

	//Crosshair Icon
	UPROPERTY()
		FCanvasIcon CrosshairTexIcon;

	//Helath2 Icon
	UPROPERTY()
		FCanvasIcon HealthIcon2;

	//Health Container Icon
	UPROPERTY()
		FCanvasIcon HealthContainerIcon;

	//Right Wing Icon
	UPROPERTY()
		FCanvasIcon RightWingIcon;

	//Left Wing Icon
	UPROPERTY()
		FCanvasIcon LeftWingIcon;

	//Score Bar Virus Icon
	UPROPERTY()
		FCanvasIcon ScoreBarVirusIcon;

	//Score Bar Anti Virus Icon
	UPROPERTY()
		FCanvasIcon ScoreBarAntiIcon;

	//Score Bar Container Bottom Icon
	UPROPERTY()
		FCanvasIcon ScoreBarContainerBotIcon;

	//Score Bar Container Top Icon
	UPROPERTY()
		FCanvasIcon ScoreBarContainerTopIcon;

	//Health bar background
	UPROPERTY()
		FCanvasIcon HealthBarBg;

	//Health bar icon
	UPROPERTY()
		FCanvasIcon HealthIcon;

	//Health bar
	UPROPERTY()
		FCanvasIcon HealthBar;

	/** Timer icon. */
	UPROPERTY()
		FCanvasIcon TimerIcon;

	/** Match timer and player position background icon. */
	UPROPERTY()
		FCanvasIcon TimePlaceBg;

	//Textures for HUD
	UPROPERTY()
		UTexture2D* HUDMainTexture;
	UPROPERTY()
		UTexture2D* HUDAssets02Texture;

	/** General offset for HUD elements. */
	float Offset;

	/** Draw player's health bar. */
	void DrawHealth();

	/** Draws weapon HUD. */
	void DrawWeaponHUD();

	/** Draws weapon crosshair. */
	void DrawCrosshair();

	/** Draws match timer and player position. */
	void DrawMatchTimerAndPosition();

	/** UI scaling factor for other resolutions than Full HD. */
	float ScaleUI;

	/** helper for getting uv coords in normalized top,left, bottom, right format */
	void MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL);

	/** Array of information strings to render (Waiting to respawn etc) */
	TArray<FCanvasTextItem> InfoItems;
};
