// ABaseItem.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h" // needed for UShapeComponent
#include "Interfaces/PickupInterface.h"
#include "PCPickupBaseItem.generated.h"

class USphereComponent;
class AFillainCharacter;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering State"),
	EIS_Equipped UMETA(DisplayName = "Equipped State"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS(BlueprintType, Blueprintable)
class HEROESANDFILLAINS_API APCPickupBaseItem : public AActor, public IPickupInterface
{
	GENERATED_BODY()

public:
	APCPickupBaseItem();
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	
	// Set at runtime by tag lookup; keep read-only to BP since we assign it in code
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup|Area",
				meta=(AllowPrivateAccess="true", EditInline="true"),
				Instanced)
	TObjectPtr<class UShapeComponent> AreaShape = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Pickup|State")
	EItemState ItemState = EItemState::EIS_Hovering;

	/*********************************************
	****                                      ****
	****    ENABLE OR DISABLE CUSTOM DEPTH    ****
	****                                      ****
	*********************************************/

	virtual void EnableCustomDepth(bool bEnable);

	virtual void ShowPickupAndInfoWidgets(bool bShow);

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	/*****************************
	***                        ***
	***   ITEM INFORMATION     ***
	***                        ***
	*****************************/
	
	// Floating hover parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldHover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverAmplitude = 20.f; // How far it moves up/down (units)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	float HoverSpeed = 2.f; // How fast it oscillates

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hover")
	bool bShouldFloatSpin = true;


	UPROPERTY()
	class UPickupWidgetComponent* FloatingWidgetComponent = nullptr;
	
	
	UPROPERTY(EditAnywhere, Category = "Pickup Effects")
	class UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere, Category = "Pickup Effects")
	USoundBase* PickupSound;

	// Niagara effect
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Khristel", meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	UPROPERTY()
	AFillainCharacter* Character;

	UPROPERTY()
	TObjectPtr<USphereComponent> SphereComp;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;



	// Let designers change the tag name per class/instance if needed
	UPROPERTY(EditDefaultsOnly, Category="Pickup|Area")
	FName AreaShapeTag = TEXT("AreaShape");

	// One place to resolve the pointer
	void ResolveAreaShapeByTag();

	// Example overlap handlers (guarded)
	UFUNCTION() virtual void OnAreaBegin(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp,
								 int32 BodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION() virtual void OnAreaEnd(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex);

public:

};
