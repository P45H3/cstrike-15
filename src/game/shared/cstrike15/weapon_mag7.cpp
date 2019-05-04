//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponMAG7 C_WeaponMAG7
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponMAG7 : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponMAG7, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponMAG7();

	virtual void PrimaryAttack();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_MAG7; }

private:

	CWeaponMAG7( const CWeaponMAG7 & );

	void MAG7Fire( float flSpread );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMAG7, DT_WeaponMAG7 )

BEGIN_NETWORK_TABLE( CWeaponMAG7, DT_WeaponMAG7 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponMAG7 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_mag7, CWeaponMAG7 );
PRECACHE_WEAPON_REGISTER( weapon_mag7 );



CWeaponMAG7::CWeaponMAG7()
{
	
}

void CWeaponMAG7::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;
	
	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		MAG7Fire( 0.3f * m_flAccuracy );
	else if (pPlayer->GetAbsVelocity().Length2D() > 170)
		MAG7Fire( 0.115f * m_flAccuracy );
	else
		MAG7Fire( 0.045f * m_flAccuracy );
}


void CWeaponMAG7::MAG7Fire( float flSpread )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CSBaseGunFire( flSpread, GetCSWpnData().m_flCycleTime, true ) )
		return;

	// Kick the gun based on the state of the player.
	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		pPlayer->KickBack (0.9, 0.45, 0.35, 0.04, 5.25, 3.5, 4);
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		pPlayer->KickBack (0.45, 0.3, 0.2, 0.0275, 4, 2.25, 7);
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		pPlayer->KickBack (0.275, 0.2, 0.125, 0.02, 3, 1, 9);
	else
		pPlayer->KickBack (0.3, 0.225, 0.125, 0.02, 3.25, 1.25, 8);
}
