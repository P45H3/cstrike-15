//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "fx_cs_shared.h"


#if defined( CLIENT_DLL )

	#define CWeaponHKP2000 C_WeaponHKP2000
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CWeaponHKP2000 : public CWeaponCSBase
{
public:
	DECLARE_CLASS( CWeaponHKP2000, CWeaponCSBase );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CWeaponHKP2000();

	virtual void Spawn();

	virtual void PrimaryAttack();
	virtual bool Deploy();

	virtual bool Reload();
	virtual void WeaponIdle();

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_HKP2000; }



private:
	
	CWeaponHKP2000( const CWeaponHKP2000 & );
	void HKP2000Fire( float flSpread );

	float m_flLastFire;
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponHKP2000, DT_WeaponHKP2000 )

BEGIN_NETWORK_TABLE( CWeaponHKP2000, DT_WeaponHKP2000 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponHKP2000 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_hkp2000, CWeaponHKP2000 );
PRECACHE_WEAPON_REGISTER( weapon_hkp2000 );



CWeaponHKP2000::CWeaponHKP2000()
{
	m_flLastFire = gpGlobals->curtime;
}


void CWeaponHKP2000::Spawn( )
{
	m_flAccuracy = 0.9;
	
	BaseClass::Spawn();
}


bool CWeaponHKP2000::Deploy( )
{
	m_flAccuracy = 0.9;

	return BaseClass::Deploy();
}

void CWeaponHKP2000::PrimaryAttack( void )
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !FBitSet( pPlayer->GetFlags(), FL_ONGROUND ) )
		HKP2000Fire( 1.5f * (1 - m_flAccuracy) );
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		HKP2000Fire( 0.255f * (1 - m_flAccuracy) );
	else if ( FBitSet( pPlayer->GetFlags(), FL_DUCKING ) )
		HKP2000Fire( 0.075f * (1 - m_flAccuracy) );
	else
		HKP2000Fire( 0.15f * (1 - m_flAccuracy) );
}

void CWeaponHKP2000::HKP2000Fire( float flSpread )
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


bool CWeaponHKP2000::Reload()
{
	if ( !DefaultPistolReload() )
		return false;

	m_flAccuracy = 0.9;
	return true;
}

void CWeaponHKP2000::WeaponIdle()
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
