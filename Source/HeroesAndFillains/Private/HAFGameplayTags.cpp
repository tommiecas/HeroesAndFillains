// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFGameplayTags.h"
#include "GameplayTagsManager.h"

FHAFGameplayTags FHAFGameplayTags::GameplayTags;

void FHAFGameplayTags::InitializeNativeGameplayTags()
{

	/*********************************
	**********************************
	****    PRIMARY ATTRIBUTES    ****
	**********************************
	*********************************/
	
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases Physical Damage")
		);

	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Primary.Intelligence"),
	FString("Increases Magical Damage")
	);

	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increases Armor and Armor Penetration, among other things...")
		);

	GameplayTags.Attributes_Primary_Dexterity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Dexterity"),
		FString("Increases Ability to Dodge and Weave in Combat")
		);

	GameplayTags.Attributes_Primary_Marksmanship = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Marksmanship"),
		FString("Increases Skill with Firearms")
		);

	GameplayTags.Attributes_Primary_Marksmanship = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Marksmanship"),
		FString("Increases Skill with Firearms")
		);

	GameplayTags.Attributes_Primary_Wisdom = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Wisdom"),
		FString("Experience and a Life Well Led Increases Multiple Attributes")
		);

	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Primary.Vigor"),
	FString("Increases Life")
	);

	GameplayTags.Attributes_Primary_Charisma = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Charisma"),
		FString("Increases Likability and Reputation... even with your Enemies!")
		);


	/***********************************
	************************************
	****    SECONDARY ATTRIBUTES    ****
	************************************
	***********************************/
	
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces Damage Taken, and Improves Block Chance")
		);

	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignores a Percentage of Enemy Armor, and increases Critical Hit Chance")
		);

	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to Cut Incoming Damage in Half")
		);

	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitChance"),
	FString("Chance to Double Damage, Plus a Bigger Critical Hit Bonus!")
	);

	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitDamage"),
	FString("Bonus Damage Added, when a Critical Hit is Scored")
	);

	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.CriticalHitResistance"),
	FString("Reduces an Attacking Enemy's Chances at Scoring a Critical Hit")
	);

	GameplayTags.Attributes_Secondary_Agility = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Agility"),
		FString("Speed and Thinking Fast on Your Feet Increases Your Chance to Dodge Attacks!")
		);

	GameplayTags.Attributes_Secondary_Flexibility = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Flexibility"),
	FString("Combined with Agility, you'll be Impossible to Hit!")
	);

	GameplayTags.Attributes_Secondary_Purity = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Purity"),
	FString("How much you're Willing to Sacrifice to Aid Those You Love with Light Majix... ")
	);

	GameplayTags.Attributes_Secondary_Corruptibility = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Corruptibility"),
		FString("The Dark Majix Speed Up Regeneration... but will Cost Your Soul...")
		);

	GameplayTags.Attributes_Secondary_Intuition = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Intuition"),
	FString("Raises the Odds You'll Find Something Valuable... Call it a Hunch!")
	);

	GameplayTags.Attributes_Secondary_Vision = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Vision"),
	FString("Increases the Odds of Making That Impossible Shot with your Firearm")
	);

	GameplayTags.Attributes_Secondary_Charm = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Charm"),
	FString("Increases the Odds of an Attacking Foe Suddenly Realizing You're Their Friend!")
	);

	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.HealthRegeneration"),
	FString("Increases the Speed you Regain Health")
	);

	GameplayTags.Attributes_Secondary_ShieldRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ShieldRegeneration"),
	FString("Increases the Speed you Fortify your Shielding")
	);

	GameplayTags.Attributes_Secondary_StaminaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.StaminaRegeneration"),
	FString("Increases the Speed you Recharge Stamina")
	);

	GameplayTags.Attributes_Secondary_MajixRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.MajixRegeneration"),
	FString("Increases the Speed you Summon More Majix")
	);

	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Secondary.MaxHealth"),
FString("Increases the Amount of Health Obtainable")
);

	GameplayTags.Attributes_Secondary_MaxShield = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Secondary.MaxShield"),
FString("Increases the Amount of Shielding Obtainable")
);

	GameplayTags.Attributes_Secondary_MaxStamina = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Secondary.MaxStamina"),
FString("Increases the Amount of Stamina Obtainable")
);
	
	GameplayTags.Attributes_Secondary_MaxMajix = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Secondary.MaxMajix"),
FString("Increases the Amount of Majix Obtainable")
);


	/***********************************
	************************************
	****    INVISIBLE ATTRIBUTES    ****
	************************************
	***********************************/
	
	GameplayTags.Attributes_Invisible_DexterityAgilityFlexibility = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Invisible.DexterityAgilityFlexibility"),
FString("How the Sausage Gets Made...Go Away")
);

	/*************************
	**************************
	****    INPUT TAGS    ****
	**************************
	*************************/
	
	GameplayTags.InputTag_LeftMouseButtonOrGamepadShoulder = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LeftMouseButtonOrGamepadShoulder"),
		FString("Input Tag for the Left Mouse Button or Gamepad Shoulder... whatever that is.")
		);
	
	GameplayTags.InputTag_RightMouseButtonOrGamepadShoulder = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RightMouseButtonOrGamepadShoulder"),
		FString("Input Tag for the Right Mouse Button Or Gamepad Shoulder... When did mice get buttons?!")
		);

	GameplayTags.InputTag_1OrDPadUp = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1OrDPadUp"),
		FString("Input Tag for the 1 Key or D-Pad Up... 1 Key... do they mean The One Key?")
		);

	GameplayTags.InputTag_2OrDPadDown = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2OrDPadDown"),
		FString("Input Tag for the 2 Button or D-Pad Down. Shouldn't it be plural? Two buttons...?") 
		);

	GameplayTags.InputTag_3OrDPadLeft = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3OrDPadLeft"),
		FString("Input Tag for the 3 Button or Deepad left. He left? Huh... good riddance then.") 
		);

	GameplayTags.InputTag_4OrDPadRight = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4OrDPadRight"),
		FString("Input Tag for the 4 Button or D-Pad Right. What does 'four-the-for' mean anyway?") 
		);

}
