//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponBizon C_WeaponBizon
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponBizon : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponBizon, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponBizon();

	virtual void PrimaryAttack();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_BIZON; }


private:

	CWeaponBizon( const CWeaponBizon & );

	void BizonFire( float flSpread );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponBizon, DT_WeaponBizon )

BEGIN_NETWORK_TABLE( CWeaponBizon, DT_WeaponBizon )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponBizon )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_bizon, CWeaponBizon );
PRECACHE_WEAPON_REGISTER( weapon_bizon );



CWeaponBizon::CWeaponBizon()
{
	
}

void CWeaponBizon::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;
	
	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		BizonFire( 0.3f * m_flAccuracy );
	else if (pPlayer->GetAbsVelocity().Length2D() > 170)
		BizonFire( 0.115f * m_flAccuracy );
	else
		BizonFire( 0.045f * m_flAccuracy );
}


void CWeaponBizon::BizonFire( float flSpread )
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
