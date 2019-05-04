//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "fx_cs_shared.h"


#if defined( CLIENT_DLL )

	#define CWeaponTEC9 C_WeaponTEC9
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponTEC9 : public CWeaponCSBase
{
public:
	DECLARE_CLASS( CWeaponTEC9, CWeaponCSBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponTEC9();

	virtual void Spawn();

	virtual void PrimaryAttack();
	virtual bool Deploy();

	virtual bool Reload();
	virtual void WeaponIdle();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_TEC9; }


private:
	
	CWeaponTEC9( const CWeaponTEC9 & );
	void TEC9Fire( float flSpread );

	float m_flLastFire;
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponTEC9, DT_WeaponTEC9 )

BEGIN_NETWORK_TABLE( CWeaponTEC9, DT_WeaponTEC9 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponTEC9 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_tec9, CWeaponTEC9 );
PRECACHE_WEAPON_REGISTER( weapon_tec9 );



CWeaponTEC9::CWeaponTEC9()
{
	m_flLastFire = gpGlobals->curtime;
}


void CWeaponTEC9::Spawn( )
{
	m_flAccuracy = 0.9;
	
	BaseClass::Spawn();
}


bool CWeaponTEC9::Deploy( )
{
	m_flAccuracy = 0.9;

	return BaseClass::Deploy();
}

void CWeaponTEC9::PrimaryAttack( void )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		TEC9Fire( 1.5f * (1 - m_flAccuracy) );
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		TEC9Fire( 0.255f * (1 - m_flAccuracy) );
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		TEC9Fire( 0.075f * (1 - m_flAccuracy) );
	else
		TEC9Fire( 0.15f * (1 - m_flAccuracy) );
}

void CWeaponTEC9::TEC9Fire( float flSpread )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	pPlayer->m_iShotsFired++;

	if (pPlayer->m_iShotsFired > 1)
		return;

	// Mark the time of this shot and determine the accuracy modifier based on the last shot fired...
	m_flAccuracy -= (0.3)*(0.325 - (gpGlobals->curtime - m_flLastFire));

	if (m_flAccuracy > 0.9)
		m_flAccuracy = 0.9;
	else if (m_flAccuracy < 0.6)
		m_flAccuracy = 0.6;

	m_flLastFire = gpGlobals->curtime;
	
	if (m_iClip1 <= 0)
	{
		if (m_bFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
		}

		return;
	}

	m_iClip1--;
	
	 pPlayer->DoMuzzleFlash();
	//SetPlayerShieldAnim();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );
		
	// Aiming
	FX_FireBullets(
		pPlayer->entindex(),
		pPlayer->Weapon_ShootPosition(),
		pPlayer->EyeAngles() + 2.0f * pPlayer->GetPunchAngle(),
		GetWeaponID(),
		Primary_Mode,
		CBaseEntity::GetPredictionRandomSeed() & 255,
		flSpread );
	
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetCSWpnData().m_flCycleTime;

	if (!m_iClip1 && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0)
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);
	}

	SetWeaponIdleTime( gpGlobals->curtime + 2 );

	//ResetPlayerShieldAnim();

	QAngle angle = pPlayer->GetPunchAngle();
	angle.x -= 2;
	pPlayer->SetPunchAngle( angle );
}


bool CWeaponTEC9::Reload()
{
	if ( !DefaultPistolReload() )
		return false;

	m_flAccuracy = 0.9;
	return true;
}

void CWeaponTEC9::WeaponIdle()
{
	if (m_flTimeWeaponIdle > gpGlobals->curtime)
		return;

	// only idle if the slid isn't back
	if (m_iClip1 != 0)
	{	
		SetWeaponIdleTime( gpGlobals->curtime + 3.0 ) ;
		SendWeaponAnim( ACT_VM_IDLE );
	}
}
