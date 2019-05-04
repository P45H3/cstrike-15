//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponSG556 C_WeaponSG556
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponSG556 : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponSG556, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponSG556();

	virtual void SecondaryAttack();
	virtual void PrimaryAttack();

	virtual float GetMaxSpeed() const;

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_SG556; }

#ifdef CLIENT_DLL
	virtual bool	HideViewModelWhenZoomed( void ) { return false; }
#endif

private:

	CWeaponSG556( const CWeaponSG556 & );

	void SG556Fire( float flSpread, bool bZoomed );

};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSG556, DT_WeaponSG556 )

BEGIN_NETWORK_TABLE( CWeaponSG556, DT_WeaponSG556 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSG556 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_sg556, CWeaponSG556 );
PRECACHE_WEAPON_REGISTER( weapon_sg556 );



CWeaponSG556::CWeaponSG556()
{
}



void CWeaponSG556::SecondaryAttack()
{
	#ifndef CLIENT_DLL
		CCSPlayer *pPlayer = GetPlayerOwner();
		if ( !pPlayer )
			return;

		if (pPlayer->GetFOV() == pPlayer->GetDefaultFOV())
		{
			pPlayer->SetFOV( pPlayer, 55, 0.2f );
		}
		else if (pPlayer->GetFOV() == 55)
		{
			pPlayer->SetFOV( pPlayer, 0, 0.15f );
		}
		else 
		{
			//FIXME: This seems wrong
			pPlayer->SetFOV( pPlayer, pPlayer->GetDefaultFOV() );
		}
	#endif

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.3;
}

void CWeaponSG556::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	bool bZoomed = pPlayer->GetFOV() < pPlayer->GetDefaultFOV();

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		SG556Fire( 0.035f + 0.45f * m_flAccuracy, bZoomed );
	else if (pPlayer->GetAbsVelocity().Length2D() > 140)
		SG556Fire( 0.035f + 0.075f * m_flAccuracy, bZoomed );
	else
		SG556Fire( 0.02f * m_flAccuracy, bZoomed );
}


void CWeaponSG556::SG556Fire( float flSpread, bool bZoomed )
{
	float flCycleTime = GetCSWpnData().m_flCycleTime;

	if ( bZoomed )
		flCycleTime = 0.135f;

	if ( !CSBaseGunFire( flSpread, flCycleTime, true ) )
		return;

	CCSPlayer *pPlayer = GetPlayerOwner();

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	if ( !pPlayer )
		return;

	if (pPlayer->GetAbsVelocity().Length2D() > 5)
		pPlayer->KickBack (1, 0.45, 0.28, 0.04, 4.25, 2.5, 7);
	else if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		pPlayer->KickBack (1.25, 0.45, 0.22, 0.18, 6, 4, 5);
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		pPlayer->KickBack (0.6, 0.35, 0.2, 0.0125, 3.7, 2, 10);
	else
		pPlayer->KickBack (0.625, 0.375, 0.25, 0.0125, 4, 2.25, 9);
}


float CWeaponSG556::GetMaxSpeed() const
{
	CCSPlayer *pPlayer = GetPlayerOwner();

	if ( !pPlayer || pPlayer->GetFOV() == pPlayer->GetDefaultFOV() )
		return BaseClass::GetMaxSpeed();
	else
		return 200; // zoomed in.
}	
