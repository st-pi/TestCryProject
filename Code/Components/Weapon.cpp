// Copyright 2017-2019 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Weapon.h"
#include "Bullet.h"

#include <CrySchematyc/Env/IEnvRegistrar.h>


namespace Game
{
	static void RegisterSpawnPointComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CWeaponComponent));
			// Functions
			{
			}
		}
	}
	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSpawnPointComponent)

	void CWeaponComponent::Shoot(QuatTS initialPosition)
	{
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

		spawnParams.vPosition = initialPosition.t;
		spawnParams.qRotation = initialPosition.q;

		const float bulletScale = 0.05f;
		spawnParams.vScale = Vec3(bulletScale);

		// Spawn the entity
		if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
		{
			// See Bullet.cpp, bullet is propelled in  the rotation and position the entity was spawned with
			pEntity->CreateComponentClass<CBulletComponent>();
		}
	}

}
