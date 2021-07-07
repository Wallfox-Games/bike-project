// Fill out your copyright notice in the Description page of Project Settings.

#include "BikePhysicalInput.h"

#include "Networking.h"
#include "SocketSubsystem.h"

#include "Misc/Paths.h" 

#include "Engine/Engine.h" 

#include "BikeGameInstance.h"

BikePhysicalInput::BikePhysicalInput(UBikeGameInstance* BikeInstanceRef, bool NewLoadDevice)
{
	GameInstanceRef = BikeInstanceRef;
	LoadDevice = NewLoadDevice;
	
	Socket = nullptr;

	RunnableThread = FRunnableThread::Create(this, TEXT("ANTPlusSocketTask"), 0, TPri_BelowNormal);
}

BikePhysicalInput::~BikePhysicalInput()
{
	Stop();
	
	if (Socket != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("deleting ant socket"));
		delete Socket;
		Socket = nullptr;
	}

	FPlatformProcess::TerminateProc(ANTProcHandle);
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

	return (Socket != nullptr);
}

uint32 BikePhysicalInput::Run()
{
	FString FullPath = FPaths::ProjectContentDir() + TEXT("ThirdParty/ANTPlus/ANT_Socket.exe");
	TCHAR* tempParam = L"0";
	if (LoadDevice) tempParam = L"1";
	bool isInEditor = (GameInstanceRef->GetWorld()->WorldType == EWorldType::PIE);
	ANTProcHandle = FPlatformProcess::CreateProc(*FullPath, tempParam, false, isInEditor, !isInEditor, NULL, 0, NULL, NULL);
	
	bool WaitingConf = true;
	
	if (Socket != nullptr)
	{
		while (Socket != nullptr && WaitingConf)
		{

			int32 BufferSize = 1;
			int32 BytesRead = 0;
			uint8 Response[1];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				if ((char)Response[0] != '0')
				{
					int DeviceType = (char)Response[0];
					GameInstanceRef->SetDeviceType(DeviceType);
					WaitingConf = false;
				}
			}

			if (WaitingConf)
			{
				GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Green, TEXT("Waiting for conf"), true);
				if (!FPlatformProcess::IsProcRunning(ANTProcHandle))
				{
					GEngine->AddOnScreenDebugMessage(2, 0.5f, FColor::Green, TEXT("Handle not valid"), true);
					FPlatformProcess::CloseProc(ANTProcHandle);
					ANTProcHandle = FPlatformProcess::CreateProc(*FullPath, tempParam, false, isInEditor, !isInEditor, NULL, 0, NULL, NULL);
				}
				// Sleep to reduce usage of system resources(nearly delta time).
				FPlatformProcess::Sleep(0.03F);
			}
		}
		GameInstanceRef->SetSensorState(true);
		// Continue updating the device while possible...
		while (Socket != nullptr)
		{	
			int32 BufferSize = 1;
			int32 BytesRead = 0;
			uint8 Response[4];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				GameInstanceRef->SetPhysicalSpeed(((float*)Response)[0]);
			}

			// Sleep to reduce usage of system resources(nearly delta time).
			FPlatformProcess::Sleep(0.03F);
		}
	}
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
