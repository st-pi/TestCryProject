// Copyright 2017-2021 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryEntitySystem/IEntitySystem.h>

namespace Game
{
	class CWeaponComponent final : public IEntityComponent
	{
	public:
		CWeaponComponent() = default;
		virtual ~CWeaponComponent() = default;

		// Reflect type to set a unique identifier for this component
		// and provide additional information to expose it in the sandbox
		static void ReflectType(Schematyc::CTypeDesc<CWeaponComponent>& desc)
		{
			desc.SetGUID("{84D58091-FCA8-4005-82D5-974FEA56BCCF}"_cry_guid);
			desc.SetEditorCategory("Game");
			desc.SetLabel("WeaponComponent");
			desc.SetDescription("This spawn point can be used to spawn entities");
		}

		void Shoot(QuatTS initialPosition);

	private:
	};
}

