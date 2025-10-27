#include "Enemies/Gnarledling.h"

AGnarledling::AGnarledling()
{
	EnemyDisplayName = FText::FromString(TEXT("a baby Gnarledling!"));
}

void AGnarledling::BeginPlay()
{
	Super::BeginPlay();

	RegisterAttackCollision(RightFootCollision);
	RegisterAttackCollision(LeftFootCollision);
}

int32 AGnarledling::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSections);
}
