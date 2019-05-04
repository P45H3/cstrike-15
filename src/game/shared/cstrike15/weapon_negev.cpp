//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponNegev C_WeaponNegev
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponNegev : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponNegev, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponNegev();

	virtual void PrimaryAttack();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_NEGEV; }


private:

	CWeaponNegev( const CWeaponNegev & );

	void NegevFire( float flSpread );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponNegev, DT_WeaponNegev )

BEGIN_NETWORK_TABLE( CWeaponNegev, DT_WeaponNegev )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponNegev )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_negev, CWeaponNegev );
PRECACHE_WEAPON_REGISTER( weapon_negev );



CWeaponNegev::CWeaponNegev()
{
}


void CWeaponNegev::PrimaryAttack( void )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		NegevFire( 0.045f + 0.5f * m_flAccuracy );
	else if (pPlayer->GetAbsVelocity().Length2D() > 140)
		NegevFire( 0.045f + 0.095f * m_flAccuracy );
	else
		NegevFire( 0.03f * m_flAccuracy );
}

void CWeaponNegev::NegevFire( float flSpread )
{
	if ( !CSBaseGunFire( flSpread, GetCSWpnData().m_flCycleTime, true ) )
		return;
	
	CCSPlayer *pPlayer = GetPlayerOwner();

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	if ( !pPlayer )
		return;

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		pPlayer->KickBack (1.8, 0.65, 0.45, 0.125, 5, 3.5, 8);
	
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		pPlayer->KickBack (1.1, 0.5, 0.3, 0.06, 4, 3, 8);
	
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		pPlayer->KickBack (0.75, 0.325, 0.25, 0.025, 3.5, 2.5, 9);
	
	else
		pPlayer->KickBack (0.8, 0.35, 0.3, 0.03, 3.75, 3, 9);
}
