// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"

// DEVNOTE TO THE FUTURE - these values require Engine Properties Postprocessing "Custom Depth-Stencil Pass" to be set to "Enabled with Stencil"
// Stencil colors for PP_Highlight
#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252
// custom collision channels
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2
#define ECC_ExcludeCharacters ECollisionChannel::ECC_GameTraceChannel3
// Custom Tag Names
#define TAG_PLAYER FName("Player")
#define TAG_ENEMY FName("Enemy")
