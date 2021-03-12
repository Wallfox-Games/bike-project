// Fill out your copyright notice in the Description page of Project Settings.

#include "BikePhysicalInput.h"
#include "Networking.h"

BikePhysicalInput::BikePhysicalInput(TArray<uint8>* SpeedsPtr)
{
	SpeedsArrayPtr = SpeedsPtr;

	FString Address = "127.0.0.1";
	int32 Port = 6123;

	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, Address, false);

	FIPv4Address ip;
	FIPv4Address::Parse(Address, ip);

	InternetAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddress->SetIp(ip.Value);
	InternetAddress->SetPort(Port);

	RunnableThread = FRunnableThread::Create(this, TEXT("ANTPlusSocketTask"), 0, TPri_BelowNormal);
}

BikePhysicalInput::~BikePhysicalInput()
{
	delete Socket;
	Socket = nullptr;

	delete RunnableThread;
	RunnableThread = nullptr;
}

bool BikePhysicalInput::Init()
{
	return true;
}

uint32 BikePhysicalInput::Run()
{
	if (Socket != nullptr)
	{
		if (Socket->Connect(*InternetAddress) == false)
		{
			return 1;
		}

		// Continue updating the device while possible...
		while (Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{	
			int32 BufferSize = 16;
			int32 BytesRead = 0;
			uint8 Response[16];

			if (Socket->Recv(Response, BufferSize, BytesRead))
			{
				ProcessSocketMessage(Response);
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
	Socket->Close();
}

void BikePhysicalInput::ProcessSocketMessage(uint8 InSocketMessage[16])
{
	uint8 NewSpeed = *InSocketMessage;
	SpeedsArrayPtr->Add(NewSpeed);
}
