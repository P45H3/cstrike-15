//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponSCAR17 C_WeaponSCAR17
	#include "c_cs_player.h"

#else

	#include "cs_player.h"
	#include "KeyValues.h"

#endif


class CWeaponSCAR17 : public CWeaponCSBaseGun
{
	
	public:
	DECLARE_CLASS( CWeaponSCAR17, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	CWeaponSCAR17();
	virtual void PrimaryAttack();
	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_SCAR17; }


	private:
	CWeaponSCAR17( const CWeaponSCAR17 & );
	void SCAR17Fire( float flSpread );
	
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSCAR17, DT_WeaponSCAR17 )

BEGIN_NETWORK_TABLE( CWeaponSCAR17, DT_WeaponSCAR17 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSCAR17 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_scar17, CWeaponSCAR17 );
PRECACHE_WEAPON_REGISTER( weapon_scar17 );

CWeaponSCAR17::CWeaponSCAR17()
{
}

void CWeaponSCAR17::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		SCAR17Fire( 0.45f * (1 - m_flAccuracy) );
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		SCAR17Fire( 0.15f );
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		SCAR17Fire( 0.04f * (1 - m_flAccuracy) );
	else
		SCAR17Fire( 0.05f * (1 - m_flAccuracy) );
}

void CWeaponSCAR17::SCAR17Fire( float flSpread )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CSBaseGunFire( flSpread, GetCSWpnData().m_flCycleTime, true ) )
		return;

	// Kick the gun based on the state of the player.
	if (pPlayer->GetAbsVelocity().Length2D() > 5)
		pPlayer->KickBack (1.0, 0.45, 0.28, 0.045, 3.75, 3, 7);
	else if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		pPlayer->KickBack (1.2, 0.5, 0.23, 0.15, 5.5, 3.5, 6);
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		pPlayer->KickBack (0.6, 0.3, 0.2, 0.0125, 3.25, 2, 7);
	else
		pPlayer->KickBack (0.65, 0.35, 0.25, 0.015, 3.5, 2.25, 7);
}
