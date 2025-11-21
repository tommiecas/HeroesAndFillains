#include "Animations/Notifies/AnimNotify_SuckStart.h"
#include "GameFramework/Actor.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Hellspawn.h"
#include "Interfaces/CombatInterface.h"

void UAnimNotify_SuckStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AHellspawn* Hellspawn = Cast<AHellspawn>(MeshComp->GetOwner());
	if (!Hellspawn) return;

	Hellspawn->StartZombieFeast();
}
