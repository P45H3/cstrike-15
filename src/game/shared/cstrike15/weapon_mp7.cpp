//========= Copyright © 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponMP7 C_WeaponMP7
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponMP7 : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponMP7, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponMP7();

	virtual void PrimaryAttack();



private:

	CWeaponMP7( const CWeaponMP7 & );

	void MP7Fire( float flSpread );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMP7, DT_WeaponMP7 )

BEGIN_NETWORK_TABLE( CWeaponMP7, DT_WeaponMP7 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponMP7 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_mp7, CWeaponMP7 );
PRECACHE_WEAPON_REGISTER( weapon_mp7 );



CWeaponMP7::CWeaponMP7()
{
	
}

void CWeaponMP7::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;
	
	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		MP7Fire( 0.3f * m_flAccuracy );
	else if (pPlayer->GetAbsVelocity().Length2D() > 170)
		MP7Fire( 0.115f * m_flAccuracy );
	else
		MP7Fire( 0.045f * m_flAccuracy );
}


void CWeaponMP7::MP7Fire( float flSpread )
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
