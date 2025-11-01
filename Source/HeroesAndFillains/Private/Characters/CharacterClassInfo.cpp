// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharClass)
{
	return CharacterClassInformation.FindChecked(CharClass);
}
