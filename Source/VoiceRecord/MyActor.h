// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Voice.h"
#include "FileManager.h"
#include "FileManager.h"
#include "OnlineSubsystemUtils.h"
#include "Sound/SoundWaveProcedural.h"
#include "Serialization/Archive.h"
#include "MyActor.generated.h"

UCLASS()
class VOICERECORD_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

    UPROPERTY()
        float VoiceCaptureVolume;

    UPROPERTY()
        bool VoiceCaptureTest;
    UPROPERTY()
        bool PlayVoiceCaptureFlag;

    UPROPERTY()
        FTimerHandle VoiceCaptureTickTimer;
    UPROPERTY()
        FTimerHandle PlayVoiceCaptureTimer;

    TSharedPtr<class IVoiceCapture> VoiceCapture;

    UPROPERTY()
        USoundWaveProcedural* VoiceCaptureSoundWaveProcedural;
    UPROPERTY()
        UAudioComponent* VoiceCaptureAudioComponent;

    UPROPERTY()
        TArray<uint8> VoiceCaptureBuffer;

    UPROPERTY()
        TArray<uint8> VoiceCaptureBufferTakenFromBinary;

    UFUNCTION()
        void    VoiceCaptureTick();

    UFUNCTION()
        void    PlayVoiceCapture();
	
    FBufferArchive ToBinary;

    //FMemoryReader FromBinary;

    UFUNCTION()
        void SaveMyRecordedVoice();

    //UFUNCTION()
    //    void LoadFromBinary();
};
