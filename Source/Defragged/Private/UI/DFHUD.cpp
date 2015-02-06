

#include "Defragged.h"
#include "UI/DFHUD.h"
#include "Player/DFCharacter.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

const float ADFHUD::MinHudScale = 0.5f;

ADFHUD::ADFHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/crosshair.crosshair'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssets02TextureOb(TEXT("/Game/UI/HUD/HUDAssets02"));
	//Andy's stuff
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/crosshair.crosshair'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/healthBar.HealthBar'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarContainerOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/healthContainer.healthContainer'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> RightWingOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/wingRight.wingRight'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ScoreBarVirusOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/scorebarVirus.scorebarVirus'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ScoreBarAntiOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/scoreBoxAnti.scoreBoxAnti'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ScoreBarContainerBotOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/scorecontainerBot.scorecontainerBot'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ScoreBarContainerTopOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/scorecontainerTop.scorecontainerTop'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> LeftWingOb(TEXT("Texture2D'/Game/MenuUI/HUD_Assets/wingLeft.WingLeft'"));

	HUDMainTexture = HUDMainTextureOb.Object;
	HUDAssets02Texture = HUDAssets02TextureOb.Object;
	
	CrosshairTex = CrosshairOb.Object;
	Health = HealthBarOb.Object;
	HealthContainer = HealthBarContainerOb.Object;
	RightWing = RightWingOb.Object;
	LeftWing = LeftWingOb.Object;
	ScoreBarVirus = ScoreBarVirusOb.Object;
	ScoreBarAnti = ScoreBarAntiOb.Object;
	ScoreBarContainerBot = ScoreBarContainerBotOb.Object;
	ScoreBarContainerTop = ScoreBarContainerTopOb.Object;
	
	CrosshairTexIcon = UCanvas::MakeIcon(CrosshairTex, 0, 0, 512, 512);
	HealthBar = UCanvas::MakeIcon(Health, 0, 0, 512, 512);
	HealthBarBg = UCanvas::MakeIcon(HealthContainer, 0, 0, 512, 512);
	RightWingIcon = UCanvas::MakeIcon(RightWing, 0, 0, 512, 512);
	LeftWingIcon = UCanvas::MakeIcon(LeftWing, 0, 0, 512, 512);
	ScoreBarVirusIcon = UCanvas::MakeIcon(ScoreBarVirus, 0, 0, 512, 512);
	ScoreBarAntiIcon = UCanvas::MakeIcon(ScoreBarAnti, 0, 0, 512, 512);
	ScoreBarContainerBotIcon = UCanvas::MakeIcon(ScoreBarContainerBot, 0, 0, 512, 512);
	ScoreBarContainerTopIcon = UCanvas::MakeIcon(ScoreBarContainerTop, 0, 0, 512, 512);

	//HealthIcon = UCanvas::MakeIcon(HUDAssets02Texture, 78, 262, 28, 28);
}


void ADFHUD::DrawHealth()
{
	ADFCharacter* MyPawn = Cast<ADFCharacter>(GetOwningPawn());
	Canvas->SetDrawColor(FColor::White);
	const float HealthPosX = (Canvas->ClipX - HealthBarBg.UL * ScaleUI) / 2;
	const float HealthPosY = Canvas->ClipY - (Offset + HealthBarBg.VL) * 2.5 * ScaleUI;
	Canvas->DrawIcon(HealthBarBg, HealthPosX, HealthPosY, ScaleUI);
	const float HealthAmount = FMath::Min(1.0f, MyPawn->Health / MyPawn->GetMaxHealth());

	FCanvasTileItem TileItem(FVector2D(HealthPosX, HealthPosY), HealthBar.Texture->Resource,
							 FVector2D(HealthBar.UL * HealthAmount * ScaleUI, HealthBar.VL * ScaleUI), FLinearColor::White);
	MakeUV(HealthBar, TileItem.UV0, TileItem.UV1, HealthBar.U, HealthBar.V, HealthBar.UL * HealthAmount, HealthBar.VL);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

void ADFHUD::DrawHUD()
{
    Super::DrawHUD();
	ScaleUI = Canvas->ClipY / 1080.0f;
	//Enforce min
	ScaleUI = FMath::Max(ScaleUI, MinHudScale);

    // find center of the canvas
    const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	//Crosshair Scaled
	const float CrosshairPosX = (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.15));
	const float CrosshairPosY = (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.15));
	Canvas->DrawIcon(CrosshairTexIcon, CrosshairPosX, CrosshairPosY, ScaleUI / 4);

	//Right Wing
	const float RightWingPosX = (Canvas->ClipX - RightWingIcon.UL * ScaleUI) * 0.95;
	const float RightWingPosY = Canvas->ClipY - (Offset + RightWingIcon.VL) * ScaleUI * 0.9;
	Canvas->DrawIcon(RightWingIcon, RightWingPosX, RightWingPosY, ScaleUI);

	//Left Wing
	const float LeftWingPosX = (Canvas->ClipX - LeftWingIcon.UL * ScaleUI) * 0.05;
	const float LeftWingPosY = Canvas->ClipY - (Offset + LeftWingIcon.VL) * ScaleUI * 0.9;
	Canvas->DrawIcon(LeftWingIcon, LeftWingPosX, LeftWingPosY, ScaleUI);

	//Score Bar
	ADFGameState* GS = Cast<ADFGameState>(GetWorld()->GameState);
	float VirusScore = 0.0f;
	float AntiScore = 0.0f;
	if (GS)
	{
		VirusScore = FMath::Min(1.0f, GS->TeamScores[1] / 25.0f);
		AntiScore = FMath::Min(1.0f, GS->TeamScores[0] / 25.0f);
	}
	const float VirusSBPosX = (Canvas->OrgX - ScoreBarContainerBotIcon.UL * ScaleUI * 0.1);
	const float VirusSBPosY = Canvas->ClipY - (Offset + ScoreBarContainerBotIcon.VL) * ScaleUI * 0.65;
	Canvas->DrawIcon(ScoreBarContainerBotIcon, VirusSBPosX, VirusSBPosY, ScaleUI);

	const float AntiSBPosX = (Canvas->OrgX - ScoreBarContainerTopIcon.UL * ScaleUI * 0.1);
	const float AntiSBPosY = Canvas->ClipY - (Offset + ScoreBarContainerTopIcon.VL) * ScaleUI * 0.8;
	Canvas->DrawIcon(ScoreBarContainerTopIcon, AntiSBPosX, AntiSBPosY, ScaleUI);

	FCanvasTileItem Virus(FVector2D(VirusSBPosX, VirusSBPosY), ScoreBarVirusIcon.Texture->Resource,
						  FVector2D(ScoreBarVirusIcon.UL * VirusScore * ScaleUI, ScoreBarVirusIcon.VL * ScaleUI), FLinearColor::White);
	MakeUV(ScoreBarVirusIcon, Virus.UV0, Virus.UV1, ScoreBarVirusIcon.U, ScoreBarVirusIcon.V, ScoreBarVirusIcon.UL * VirusScore, ScoreBarVirusIcon.VL);
	Virus.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Virus);

	FCanvasTileItem Anti(FVector2D(AntiSBPosX, AntiSBPosY), ScoreBarAntiIcon.Texture->Resource,
						 FVector2D(ScoreBarAntiIcon.UL * AntiScore * ScaleUI, ScoreBarAntiIcon.VL * ScaleUI), FLinearColor::White);
	MakeUV(ScoreBarAntiIcon, Anti.UV0, Anti.UV1, ScoreBarAntiIcon.U, ScoreBarAntiIcon.V, ScoreBarAntiIcon.UL * AntiScore, ScoreBarAntiIcon.VL);
	Anti.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Anti);

	// Empty the info item array
	InfoItems.Empty();


	ADFCharacter* MyPawn = Cast<ADFCharacter>(GetOwningPawn());
	if (MyPawn)
		DrawHealth();
}

void ADFHUD::MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL)
{
	if (Icon.Texture)
	{
		const float Width = Icon.Texture->GetSurfaceWidth();
		const float Height = Icon.Texture->GetSurfaceHeight();
		UV0 = FVector2D(U / Width, V / Height);
		UV1 = UV0 + FVector2D(UL / Width, VL / Height);
	}
}