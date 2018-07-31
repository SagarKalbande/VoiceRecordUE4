// Fill out your copyright notice in the Description page of Project Settings.

#include "VoiceRecord.h"
#include "MyActor.h"
#include "FileManager.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    VoiceCapture = FVoiceModule::Get().CreateVoiceCapture();
    VoiceCapture->Start();

    VoiceCaptureAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceCaptureAudioComponent"));
    VoiceCaptureAudioComponent->SetupAttachment(RootComponent);
    VoiceCaptureAudioComponent->bAutoActivate = true;
    VoiceCaptureAudioComponent->bAlwaysPlay = true;
    VoiceCaptureAudioComponent->PitchMultiplier = 0.0f;
    VoiceCaptureAudioComponent->VolumeMultiplier = 4.f;

    

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

    
    VoiceCaptureSoundWaveProcedural = NewObject<USoundWaveProcedural>();
    VoiceCaptureSoundWaveProcedural->SampleRate = 60650;
    VoiceCaptureSoundWaveProcedural->NumChannels = 1;
    VoiceCaptureSoundWaveProcedural->Duration = INDEFINITELY_LOOPING_DURATION;
    VoiceCaptureSoundWaveProcedural->SoundGroup = SOUNDGROUP_Voice;
    VoiceCaptureSoundWaveProcedural->bLooping = false;
    VoiceCaptureSoundWaveProcedural->bProcedural = true;
    VoiceCaptureSoundWaveProcedural->Pitch = 0.60f;
    VoiceCaptureSoundWaveProcedural->Volume = 5.f;

 //   GetWorldTimerManager().SetTimer(PlayVoiceCaptureTimer, this, &AMyActor::SaveMyRecordedVoice, 5.f, false, 5.0f);
//    GetWorldTimerManager().SetTimer(PlayVoiceCaptureTimer, this, &AMyActor::LoadFromBinary, 10.f, false, 10.0f);  calling this immediately fails!! So i have called it in the function.

    }    

// Called every frame
void AMyActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

    VoiceCaptureTick();
}

void AMyActor::VoiceCaptureTick()
{
    if (!VoiceCapture.IsValid())
        return;

    uint32 VoiceCaptureBytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(VoiceCaptureBytesAvailable);


    uint32 VoiceCaptureReadBytes = 100;

    VoiceCaptureBuffer.SetNumUninitialized(VoiceCaptureBytesAvailable);
    VoiceCapture->GetVoiceData(VoiceCaptureBuffer.GetData(), VoiceCaptureBytesAvailable, VoiceCaptureReadBytes);
    VoiceCaptureSoundWaveProcedural->QueueAudio(VoiceCaptureBuffer.GetData(), VoiceCaptureReadBytes);
    VoiceCaptureAudioComponent->SetSound(VoiceCaptureSoundWaveProcedural);
    VoiceCaptureAudioComponent->Play();
    if (VoiceCaptureAudioComponent->IsPlaying())
    {
        UE_LOG(LogTemp, Warning, TEXT("Playing"));
    }
}

