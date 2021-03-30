// Fill out your copyright notice in the Description page of Project Settings.

#include "BikePhysicalInput.h"

#include "Networking.h"
#include "SocketSubsystem.h"

#include "Engine/Engine.h" 

#include "BikeGameInstance.h"

BikePhysicalInput::BikePhysicalInput(UBikeGameInstance* BikeInstanceRef)
{
	GameInstanceRef = BikeInstanceRef;

	Socket = nullptr;

	RunnableThread = FRunnableThread::Create(this, TEXT("ANTPlusSocketTask"), 0, TPri_BelowNormal);
}

BikePhysicalInput::~BikePhysicalInput()
{
	Stop();
	
	if (Socket != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("deleting socket"));
		delete Socket;
		Socket = nullptr;
	}

	delete RunnableThread;
	RunnableThread = nullptr;
}

bool BikePhysicalInput::Init()
{
	FString Address = "127.0.0.1";
	int32 Port = 6123;
	FIPv4Address ip;
	FIPv4Address::Parse(Address, ip);

	Socket = FUdpSocketBuilder(TEXT("ANTPlus Socket")).BoundToAddress(ip).BoundToPort(Port).Build();

	if (Socket == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Socket null!"));
	}

	return (Socket != nullptr);
}

uint32 BikePhysicalInput::Run()
{
	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(TEXT("../ANTPlus/ANT_Socket.exe"));	
	TCHAR* tempParam = L" ";
	FProcHandle tempProcHandle = FPlatformProcess::CreateProc(*FullPath, tempParam, false, true, false, NULL, 0, NULL, NULL);
	
	if (Socket != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Socket open!"));

		// Continue updating the device while possible...
		while (Socket != nullptr)
		{	
			int32 BufferSize = 4;
			int32 BytesRead = 0;
			uint8 Response[4];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				ProcessSocketMessage(Response);
			}

			// Sleep to reduce usage of system resources(nearly delta time).
			FPlatformProcess::Sleep(0.03F);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Socket null!"));
	return 0;
}

void BikePhysicalInput::Stop()
{
	// This will make the socket connection state different then SCS_Connected and
	// It will break the while loop in Run method.
	if (Socket != nullptr)
	{
		Socket->Close();
	}
}

void BikePhysicalInput::ProcessSocketMessage(uint8 InSocketMessage[4])
{
	unsigned short eventTime = ((unsigned short*)InSocketMessage)[0];
	unsigned short revCount = ((unsigned short*)InSocketMessage)[1];

	GameInstanceRef->FillArrays(eventTime, revCount);
}