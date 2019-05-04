//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbasegun.h"


#if defined( CLIENT_DLL )

	#define CWeaponSCAR20 C_WeaponSCAR20
	#include "c_cs_player.h"

#else

	#include "cs_player.h"
	#include "KeyValues.h"

#endif


class CWeaponSCAR20 : public CWeaponCSBaseGun
{
public:
	DECLARE_CLASS( CWeaponSCAR20, CWeaponCSBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponSCAR20();

	virtual void Spawn();
	virtual void SecondaryAttack();
	virtual void PrimaryAttack();
	virtual bool Reload();
	virtual bool Deploy();

	virtual float GetMaxSpeed() const;

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_SCAR20; }


private:

	CWeaponSCAR20( const CWeaponSCAR20 & );

	void SCAR20Fire( float flSpread );

	float m_flLastFire;
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSCAR20, DT_WeaponSCAR20 )

BEGIN_NETWORK_TABLE( CWeaponSCAR20, DT_WeaponSCAR20 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSCAR20 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_scar20, CWeaponSCAR20 );
PRECACHE_WEAPON_REGISTER( weapon_scar20 );



CWeaponSCAR20::CWeaponSCAR20()
{
	m_flLastFire = gpGlobals->curtime;
}

void CWeaponSCAR20::Spawn()
{
	BaseClass::Spawn();
	m_flAccuracy = 0.98;
}


void CWeaponSCAR20::SecondaryAttack()
{
	#ifndef CLIENT_DLL
		CCSPlayer *pPlayer = GetPlayerOwner();
		if ( !pPlayer )
			return;

		if (pPlayer->GetFOV() == pPlayer->GetDefaultFOV())
		{
			pPlayer->SetFOV( pPlayer, 40, 0.3f );
		}
		else if (pPlayer->GetFOV() == 40)
		{
			pPlayer->SetFOV( pPlayer, 15, 0.05f );
		}
		else if (pPlayer->GetFOV() == 15)
		{
			pPlayer->SetFOV( pPlayer, pPlayer->GetDefaultFOV(), 0.1f );
		}

		pPlayer->ResetMaxSpeed();
	#endif

#ifndef CLIENT_DLL
	// If this isn't guarded, the sound will be emitted twice, once by the server and once by the client.
	// Let the server play it since if only the client plays it, it's liable to get played twice cause of
	// a prediction error. joy.
	EmitSound( "Default.Zoom" ); // zoom sound.

	// let the bots hear the rifle zoom
	IGameEvent * event = gameeventmanager->CreateEvent( "weapon_zoom" );
	if( event )
	{
		event->SetInt( "userid", pPlayer->GetUserID() );
		gameeventmanager->FireEvent( event );
	}
#endif

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.3;   
	m_zoomFullyActiveTime = gpGlobals->curtime + 0.3; // The worst zoom time from above.  
}

void CWeaponSCAR20::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		SCAR20Fire( 0.45f * (1 - m_flAccuracy) );
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		SCAR20Fire( 0.15f );
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		SCAR20Fire( 0.04f * (1 - m_flAccuracy) );
	else
		SCAR20Fire( 0.05f * (1 - m_flAccuracy) );
}

void CWeaponSCAR20::SCAR20Fire( float flSpread )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	// If we are not zoomed in, or we have very recently zoomed and are still transitioning, the bullet diverts more.
	if (pPlayer->GetFOV() == pPlayer->GetDefaultFOV() || (gpGlobals->curtime < m_zoomFullyActiveTime))
		flSpread += 0.025;

	// Mark the time of this shot and determine the accuracy modifier based on the last shot fired...
	m_flAccuracy = 0.65 + (0.35) * (gpGlobals->curtime - m_flLastFire);	

	if (m_flAccuracy > 0.98)
		m_flAccuracy = 0.98;

	m_flLastFire = gpGlobals->curtime;

	if ( !CSBaseGunFire( flSpread, GetCSWpnData().m_flCycleTime, true ) )
		return;

	QAngle angle = pPlayer->GetPunchAngle();
	angle.x -= SharedRandomFloat("SCAR20PunchAngleX", 0.75, 1.25 ) + ( angle.x / 4 );
	angle.y += SharedRandomFloat("SCAR20PunchAngleY", -0.75, 0.75 );
	pPlayer->SetPunchAngle( angle );
}

bool CWeaponSCAR20::Reload()
{
	bool ret = BaseClass::Reload();
	
	m_flAccuracy = 0.98;
	
	return ret;
}

bool CWeaponSCAR20::Deploy()
{
	bool ret = BaseClass::Deploy();
	
	m_flAccuracy = 0.98;
	
	return ret;
}

float CWeaponSCAR20::GetMaxSpeed() const
{
	CCSPlayer *pPlayer = GetPlayerOwner();

	if ( !pPlayer || pPlayer->GetFOV() == 90 )
		return BaseClass::GetMaxSpeed();
	else
		return 150; // zoomed in
}
