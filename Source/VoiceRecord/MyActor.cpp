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
    VoiceCaptureSoundWaveProcedural->SampleRate = 16000;
    VoiceCaptureSoundWaveProcedural->NumChannels = 1;
    VoiceCaptureSoundWaveProcedural->Duration = INDEFINITELY_LOOPING_DURATION;
    VoiceCaptureSoundWaveProcedural->SoundGroup = SOUNDGROUP_Voice;
    VoiceCaptureSoundWaveProcedural->bLooping = false;
    VoiceCaptureSoundWaveProcedural->bProcedural = true;
    VoiceCaptureSoundWaveProcedural->Pitch = 0.85f;
    VoiceCaptureSoundWaveProcedural->Volume = 5.f;

    GetWorldTimerManager().SetTimer(PlayVoiceCaptureTimer, this, &AMyActor::SaveMyRecordedVoice, 5.f, false, 5.0f);
//    GetWorldTimerManager().SetTimer(PlayVoiceCaptureTimer, this, &AMyActor::LoadFromBinary, 10.f, false, 10.0f);  calling this immediately fails!! So i have called it in the function.
//    GetWorldTimerManager().SetTimer(PlayVoiceCaptureTimer, this, &AMyActor::PlayVoiceCapture, 15.f, false, 15.0f);

    //FileMan = NewObject<IFileManager>();

    //FString PathString = FString(TEXT("C:/Users/VizExperts/Desktop/"));
    //const TCHAR* PathTchar = *PathString;

    //FileMan->MakeDirectory(PathTchar, false);

    //FString MyString = FString(TEXT("C:/Users/VizExperts/Desktop/hola/hola.txt"));
    //const TCHAR* PathString = *MyString;

    //IPlatformFile& Platformfile = FPlatformFileManager::Get().GetPlatformFile();

    //if (Platformfile.FileExists(PathString))
    //{
    //    const int64 FileSize = Platformfile.FileSize(PathString);
    //    UE_LOG(LogTemp, Warning, TEXT("File size is: %d"), FileSize);
    //}

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

    VoiceCaptureBuffer.Reset();
    PlayVoiceCaptureFlag = false;

    if (CaptureState == EVoiceCaptureState::Ok && VoiceCaptureBytesAvailable > 0)
    {
        int16_t VoiceCaptureSample;
        uint32 VoiceCaptureReadBytes;
        float VoiceCaptureTotalSquared = 0;

        VoiceCaptureBuffer.SetNumUninitialized(VoiceCaptureBytesAvailable);

        VoiceCapture->GetVoiceData(VoiceCaptureBuffer.GetData(), VoiceCaptureBytesAvailable, VoiceCaptureReadBytes);

        for (uint32 i = 0; i < (VoiceCaptureReadBytes / 2); i++)
        {
            VoiceCaptureSample = (VoiceCaptureBuffer[i * 2 + 1] << 8) | VoiceCaptureBuffer[i * 2];
            VoiceCaptureTotalSquared += ((float)VoiceCaptureSample * (float)VoiceCaptureSample);
        }

        float VoiceCaptureMeanSquare = (2 * (VoiceCaptureTotalSquared / VoiceCaptureBuffer.Num()));
        float VoiceCaptureRms = FMath::Sqrt(VoiceCaptureMeanSquare);
        float VoiceCaptureFinalVolume = ((VoiceCaptureRms / 32768.0) * 200.f);

        VoiceCaptureVolume = VoiceCaptureFinalVolume;

        VoiceCaptureSoundWaveProcedural->QueueAudio(VoiceCaptureBuffer.GetData(), VoiceCaptureReadBytes);
        VoiceCaptureAudioComponent->SetSound(VoiceCaptureSoundWaveProcedural);

        PlayVoiceCaptureFlag = true;
    }

    ToBinary << VoiceCaptureBuffer;
}

void AMyActor::PlayVoiceCapture()
{
    //if (!PlayVoiceCaptureFlag)
    //{
    //    VoiceCaptureAudioComponent->FadeOut(0.3f, 0.f);
    //    return;
    //}

    //if (VoiceCaptureAudioComponent->IsPlaying())
    //    return;

    //VoiceCaptureAudioComponent->Play();



}

void AMyActor::SaveMyRecordedVoice()
{
    FString FilePath = FString(TEXT("C:/Users/VizExperts/Desktop/hola/mysavedbinary.exe"));
    const TCHAR* PathString = *FilePath;

    if (FFileHelper::SaveArrayToFile(ToBinary, PathString))
    {
        ToBinary.FlushCache();
        ToBinary.Empty();
    }
	
    ToBinary.FlushCache();
    ToBinary.Empty();


    UE_LOG(LogTemp, Warning, TEXT("File Saved"));

    GetWorldTimerManager().SetTimer(PlayVoiceCaptureTimer, this, &AMyActor::LoadFromBinary, 5.f, false, 5.0f);
}

void AMyActor::LoadFromBinary()
{
    FString FilePath = FString(TEXT("C:/Users/VizExperts/Desktop/hola/mysavedbinary.exe"));
    const TCHAR* PathString = *FilePath;
    bool Loaded = FFileHelper::LoadFileToArray(VoiceCaptureBufferTakenFromBinary, *FilePath);
    if (Loaded)
    {
        UE_LOG(LogTemp, Warning, TEXT("File Loaded Successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("File Loading failed"));

    }
}

