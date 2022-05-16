// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "FIT3094_A1_CodeGameModeBase.h"

#include "Misc/FileHelper.h"

TArray<FString> AFIT3094_A1_CodeGameModeBase::GetMapArray()
{
	TArray<FString> MapArray;


	//FString MapText = GetMapText(FString MapName);
	FString MapText = GetRandomMapText();
	MapText.ParseIntoArrayLines(MapArray);

	return MapArray;
}

FString AFIT3094_A1_CodeGameModeBase::GetRandomMapText()
{
	TArray<FString> MapFiles = GetMapFileList();

	int32 MapPosition = FMath::RandRange(0, MapFiles.Num() - 1);
	FString MapPath = MapFiles[MapPosition];

	FString MapText;
	FFileHelper::LoadFileToString(MapText, *MapPath);

	return MapText;
}

TArray<FString> AFIT3094_A1_CodeGameModeBase::GetMapFileList()
{
		TArray<FString> MapFiles;

		const FString MapsDir = FPaths::ProjectContentDir() + "MapFiles/";
		FPlatformFileManager::Get().GetPlatformFile().FindFiles(MapFiles, *MapsDir, nullptr);
		
		return MapFiles;
}




