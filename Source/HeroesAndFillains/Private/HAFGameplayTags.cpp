// Fill out your copyright notice in the Description page of Project Settings.


#include "HAFGameplayTags.h"
#include "GameplayTagsManager.h"

// Registers safely during module startup; no static-init order issues.
UE_DEFINE_GAMEPLAY_TAG(TAG_VitalEffects_StaminaCost, "VitalEffects.StaminaCost");
UE_DEFINE_GAMEPLAY_TAG(TAG_VitalEffects_StaminaRegen, "VitalEffects.StaminaRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_VitalEffects_MajixCost, "VitalEffects.MajixCost");
UE_DEFINE_GAMEPLAY_TAG(TAG_VitalEffects_MajixRegen, "VitalEffects.MajixRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_VitalEffects_ShieldRegen, "VitalEffects.ShieldRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_VitalEffects_HealthRegen, "VitalEffects.HealthRegen");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage_Shield,      "Damage.Shield");
UE_DEFINE_GAMEPLAY_TAG(TAG_Damage_Health,      "Damage.Health");
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

	GameplayTags.Attributes_Secondary_Speed = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Secondary.Speed"),
	FString("Increases how Fast You Walk, Run, and Jump")
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

	/*******************************
	********************************
	****    VITAL ATTRIBUTES    ****
	********************************
	*******************************/
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.Health"),
	FString("Amount of Damage Incurred Before Death")
	);

	GameplayTags.Attributes_Vital_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Vital.Shield"),
	FString("Amount of Shielding Destroyed Before it Affects Your Health")
	);

	GameplayTags.Attributes_Vital_Stamina = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Vital.Stamina"),
FString("Amount of Times One Can Dodge an Attack Before Exhaustion Sets In, And No Attack is Dodgeable")
);

	GameplayTags.Attributes_Vital_Majix = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Attributes.Vital.Majix"),
FString("Amount of Majixal Ability One Has Left Before They Are Unable To Cast Spells") 
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

	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Duh. Damage!") 
		);
	
	/***************************
	****************************
	****    DAMAGE TYPES    ****
	****************************
	***************************/

	GameplayTags.Damage_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Burn"),
		FString("Fire burns... in case you didn't know.") 
		);

	GameplayTags.Damage_Shock = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Shock"),
		FString("Lightning electrocutes... you'll see soon enough.") 
		);

	GameplayTags.Damage_CorruptChaos = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.CorruptChaos"),
		FString("Chaos corrupts... is your soul worth it?")
		);

	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical damage...because sometimes nothing says it better than a good punch in the face.")
		);

	GameplayTags.Damage_PureOrder = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.PureOrder"),
		FString("Pure Order... always stay in line and obey.")
		);

	GameplayTags.Damage_Frostbite = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Frostbite"),
		FString("Frostbite... ice, ice, baby.")
		);

	GameplayTags.Damage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Poison"),
		FString("Poison... the clock is ticking.")
		);

	GameplayTags.Damage_Paralysis = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Paralysis"),
		FString("Paralysis... hope you like it here, because you're never leaving.")
		);

	/**************************
	***************************
	****    RESISTANCES    ****
	***************************
	**************************/

	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Fire"),
		FString("Reduces Burn Damage Taken from Fire")
		);

	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Lightning"),
		FString("Reduces Shock Damage Taken from Lightning")
		);

	GameplayTags.Attributes_Resistance_ChaosMajix = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.ChaosMajix"),
		FString("Reduces Corrupt Chaos Damage Taken from Majix")
		);

	GameplayTags.Attributes_Resistance_MeleeAttacks = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.MeleeAttacks"),
		FString("Reduces Physical Damage Taken from Melee Attacks")
		);

	GameplayTags.Attributes_Resistance_RuleOfOrder = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.RuleOfOrder"),
		FString("Reduces Pure Order Damage Taken from the Rule of Order")
		);

	GameplayTags.Attributes_Resistance_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Ice"),
		FString("Reduces Frostbite Damage Taken from Ice")
		);

	GameplayTags.Attributes_Resistance_Toxicity = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Toxicity"),
		FString("Reduces Poison Damage Taken from Toxic Substances")
		);

	GameplayTags.Attributes_Resistance_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Stun"),
		FString("Reduces Paralysis Damage Taken from Being Stunned")
		);
	

	/*************************************************
	**************************************************
	****    MAP OF DAMAGE TYPES TO RESISTANCES    ****
	**************************************************
	*************************************************/

	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Burn, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Shock, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_CorruptChaos, GameplayTags.Attributes_Resistance_ChaosMajix);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_MeleeAttacks);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_PureOrder, GameplayTags.Attributes_Resistance_RuleOfOrder);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Frostbite, GameplayTags.Attributes_Resistance_Ice);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Poison, GameplayTags.Attributes_Resistance_Toxicity);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Paralysis, GameplayTags.Attributes_Resistance_Stun);
	
	/**********************
	***********************
	****    EFFECTS    ****
	***********************
	**********************/
	
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("A Reaction to Getting Hit") 
		);

	GameplayTags.Damage_IncomingDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.IncomingDamage"),
		FString("Damage that is being dealt to you") 
		);

	/************************
	*************************
	****    ABILITIES    ****
	*************************
	************************/
	
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"),
		FString("When You're Pissed Off Enough to Fight Back") 
		);

	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("When You Get Others to Do Your Dirty Work for You") 
		);

	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities.Fire.FireBolt"),
	FString("When You Just Want to Hit Someone in the Face with a Fireball") 
	);

	/***********************
	************************
	****    COOLDOWN    ****
	************************
	***********************/

	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Cooldown.Fire.FireBolt"),
	FString("You can't just shoot those things over and over again and expect it to never end!") 
	);

	/*****************************
	******************************
	****    COMBAT SOCKETS    ****
	******************************
	*****************************/
	
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.Weapon"),
	FString("When Your Weapon Is All You've Got") 
	);

	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.LeftHand"),
	FString("When Your Left Hand Is All You've... Wait a Minute...") 
	);

	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("CombatSocket.RightHand"),
	FString("When Your Right Hand Is All You've Okay, There's GOT To Be A Better Way To Phrase This...") 
	);

	GameplayTags.CombatSocket_LeftFoot = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.LeftFoot"),
		FString("When Your Left Foot Is All You've Got") 
		);
	
	GameplayTags.CombatSocket_RightFoot = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.RightFoot"),
		FString("When Your Right Foot Is All You've Got")  
		);

	GameplayTags.CombatSocket_BothHands = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.BothHands"),
		FString("When Your Hands Are All You've Got")  
		);
		
	GameplayTags.CombatSocket_BothFeet = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.BothFeet"),
		FString("When Your Feet Are All You've Got")  
		);

	GameplayTags.CombatSocket_HandsAndFeet = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.HandsAndFeet"),
		FString("When Your Hands and Feet Are All You've Got... then you pretty much have it all, so what's the problem...?")  
		);

	GameplayTags.CombatSocket_WeaponAndFeet = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.WeaponAndFeet"),
		FString("When Your Weapon and Feet Are All You've... wait... really...?")  
		);

	GameplayTags.CombatSocket_WeaponAndHands = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.WeaponAndHands"),
		FString("When Your Weapon and Your Hands Are All... this is just getting silly now...")
		);

	GameplayTags.CombatSocket_WeaponAndHandsAndFeet = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.WeaponAndHandsAndFeet"),
		FString("When Your Weapon and Your Hands and YourFeet Are All-- I QUIT!")  
		);

	GameplayTags.CombatSocket_Teeth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Teeth"),
		FString("When Your Mouth is All You've... Nope. This is where I draw the line. Enough.")  
		);

	/***********************
	************************
	****    MONTAGES    ****
	************************
	***********************/

	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Montage.Attack.1"),
	FString("1st Attack Montage")  
	);

	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.2"),
FString("2st Attack Montage")  
);

	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.3"),
FString("3rd Attack Montage")  
);

	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.4"),
FString("4th Attack Montage")  
);

	GameplayTags.Montage_Attack_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.5"),
FString("5th Attack Montage")  
);

	GameplayTags.Montage_Attack_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.6"),
FString("6th Attack Montage")  
);

	GameplayTags.Montage_Attack_7 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.7"),
FString("7th Attack Montage")  
);

	GameplayTags.Montage_Attack_8 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.8"),
FString("8th Attack Montage")  
);

	GameplayTags.Montage_Attack_9 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.9"),
FString("9th Attack Montage")  
);

	GameplayTags.Montage_Attack_10 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.10"),
FString("10th Attack Montage")  
);

	GameplayTags.Montage_Attack_11 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.11"),
FString("11th Attack Montage")  
);

	GameplayTags.Montage_Attack_12 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.12"),
FString("12th Attack Montage")  
);

	GameplayTags.Montage_Attack_13 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.13"),
FString("13th Attack Montage")  
);

	GameplayTags.Montage_Attack_14 = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Montage.Attack.14"),
FString("14th Attack Montage")  
);

	GameplayTags.Montage_Attack_15 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.15"),
		FString("15th Attack Montage")  
		);

	GameplayTags.Montage_Attack_16 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.16"),
		FString("16th Attack Montage")  
		);

	GameplayTags.Montage_Attack_17 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.17"),
		FString("17th Attack Montage")  
		);

	GameplayTags.Montage_Attack_18 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.18"),
		FString("18th Attack Montage")  
		);

	GameplayTags.Montage_Attack_19 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.19"),
		FString("19th Attack Montage")  
		);

	GameplayTags.Montage_Attack_20 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.20"),
		FString("20th Attack Montage")  
		);
}