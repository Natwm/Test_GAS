// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TTSBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraDataInterfaceEmitterBinding.h"
#include "Components/CapsuleComponent.h"
#include "Game/TTSDestroyAllCreatureGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/TTSGridManager.h"
#include "TTShooter/TTShooter.h"


class ATTSGridManager;
// Sets default values
ATTSBaseCharacter::ATTSBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

UAbilitySystemComponent* ATTSBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

//Todo:: Update those methods don't seems to work
void ATTSBaseCharacter::HighlightActor()
{
	EnsureDynamicMaterial(); // S'assurer que l'instance dynamique est prête

	if (DynamicColor)
	{
		DynamicColor->SetVectorParameterValue(TEXT("ColorMultiplier"), FVector(255, 10, 10));
		UE_LOG(LogTemp, Warning, TEXT("HighlightActor %s."), *GetName());
	}
}

void ATTSBaseCharacter::UnHighlightActor()
{
	EnsureDynamicMaterial(); // S'assurer que l'instance dynamique est prête

	if (DynamicColor)
	{
		DynamicColor->SetVectorParameterValue(TEXT("ColorMultiplier"), FVector(0, 0, 0));
		UE_LOG(LogTemp, Warning, TEXT("UnHighlightActor %s."), *GetName());
	}
}

void ATTSBaseCharacter::SelectedActor()
{
	EnsureDynamicMaterial(); // S'assurer que l'instance dynamique est prête
    
    	if (DynamicColor)
    	{
    		DynamicColor->SetVectorParameterValue(TEXT("ColorMultiplier"), FVector(52, 52, 52));
    		UE_LOG(LogTemp, Warning, TEXT("UnHighlightActor %s."), *GetName());
    	}
}

// Called when the game starts or when spawned
void ATTSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterOnGrid();
}

void ATTSBaseCharacter::InitAbilityActorInfo()
{
}

void ATTSBaseCharacter::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const
{
	// Check if element is valid
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	//Creation of the Gameplay Effect Context
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle =  GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Level,ContextHandle);

	//Applay the game effect Context on self
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(),GetAbilitySystemComponent());

}

void ATTSBaseCharacter::InitializeDefaultAttribute() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes,1);
	ApplyEffectToSelf(DefaultSecondaryAttributes,1);
	ApplyEffectToSelf(DefaultVitalAttributes,1);
}

void ATTSBaseCharacter::AddCharacterAbilities() const
{
	if(!HasAuthority())
		return;
	
	//UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	//AuraASC->AddCharacterAbilities(StartupAbilities);
}

void ATTSBaseCharacter::SetCharacterOnGrid()
{
	ATTSGridManager* grid = Cast<ATTSGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATTSGridManager::StaticClass()));
	//grid->SetCharacterOnGrid(this);

	//todo :: check error avec gamemode
	//ATTSDestroyAllCreatureGameMode* GameMode = Cast<ATTSDestroyAllCreatureGameMode>(GetWorld()->GetAuthGameMode());
	//GameMode->UpdateGridCharacterData(this,false,true);

}

void ATTSBaseCharacter::EnsureDynamicMaterial()
{
	if (!DynamicColor)
	    {
	        UMaterialInterface* Material = GetMesh()->GetMaterial(0); // Récupère le matériau initial
	        if (Material)
	        {
	            DynamicColor = UMaterialInstanceDynamic::Create(Material, this); // Crée une instance dynamique
	            GetMesh()->SetMaterial(0, DynamicColor); // Applique la nouvelle instance
	        }
	    }
}

// Called every frame
void ATTSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATTSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
