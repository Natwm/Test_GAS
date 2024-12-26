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
	UMaterialInterface* Material = GetMesh()->GetMaterial(0); // 0 = premier slot de matériau
	if (!Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("No material found on the mesh component."));
		return;
	}

	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
	if (!DynamicMaterial)
	{
		// Créer une instance dynamique du matériau
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, GetMesh());
		if (!DynamicMaterial)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create dynamic material instance."));
			return;
		}

		// Assigner l'instance dynamique au composant
		GetMesh()->SetMaterial(0, DynamicMaterial);
	}

	// Modifier la valeur du paramètre
	DynamicMaterial->SetVectorParameterValue("Color Multiply", FVector(255,25,25));
}

void ATTSBaseCharacter::UnHighlightActor()
{
	UMaterialInterface* Material = GetMesh()->GetMaterial(0); // 0 = premier slot de matériau
	if (!Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("No material found on the mesh component."));
		return;
	}

	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
	if (!DynamicMaterial)
	{
		// Créer une instance dynamique du matériau
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, GetMesh());
		if (!DynamicMaterial)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create dynamic material instance."));
			return;
		}

		// Assigner l'instance dynamique au composant
		GetMesh()->SetMaterial(0, DynamicMaterial);
	}

	// Modifier la valeur du paramètre
	DynamicMaterial->SetVectorParameterValue("Color Multiply", FVector(0,0,25));
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
	grid->SetCharacterOnGrid(this);

	//todo :: check error avec gamemode
	//ATTSDestroyAllCreatureGameMode* GameMode = Cast<ATTSDestroyAllCreatureGameMode>(GetWorld()->GetAuthGameMode());
	//GameMode->UpdateGridCharacterData(this,false,true);

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
