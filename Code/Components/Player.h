// Copyright 2017-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>

////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////
namespace Game
{
	class CPlayerComponent final : public IEntityComponent
	{
	public:
		CPlayerComponent() = default;
		virtual ~CPlayerComponent() = default;

		virtual void Initialize() override;

		virtual Cry::Entity::EventFlags GetEventMask() const override;
		virtual void ProcessEvent(const SEntityEvent& event) override;

		// Reflect type to set a unique identifier for this component
		static void ReflectType(Schematyc::CTypeDesc<CPlayerComponent>& desc)
		{
			desc.SetGUID("{63F4C0C6-32AF-4ACB-8FB0-57D45DD14725}"_cry_guid);
			desc.AddMember(&CPlayerComponent::m_movementSpeed, 'pms', "playermovespeed", "Player Movement Speed", "Sets the Player MS", ZERO);
			desc.AddMember(&CPlayerComponent::m_rotationSpeed, 'prs', "playerrotationspeed", "Player Rotation Speed", "Sets the speed of the players rotationPlayer", ZERO);
			desc.AddMember(&CPlayerComponent::m_cameraDefaultPos, 'cdp', "cameradefaultpos", "Camera Default Position", "Sets the camera position", ZERO);
			desc.AddMember(&CPlayerComponent::m_rotationLimitsMinPitch, 'cpmi', "camerapitchmin", "Camera Pitch Min", "Minimum rotation value for camera pitch", -0.85f);
			desc.AddMember(&CPlayerComponent::m_rotationLimitsMaxPitch, 'cpm', "camerapitchmax", "Camera Pitch Max", "Maximum rotation value for camera pitch", 1.5f);
		}

	protected:

	private:
		void InitializeInput();
		void UpdatePlayerMovement();
		void UpdateCameraRotation();

	private:
		Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;
		Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;
		Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponnet = nullptr;
		Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent = nullptr;

		Quat m_lookOrientation;

		Vec3 m_cameraDefaultPos;

		Vec2 m_movementDelta;
		Vec2 m_mouseDeltaRotation;

		float m_movementSpeed = 0.0f;
		float m_rotationSpeed = 0.0f;

		float m_rotationLimitsMinPitch = 0.0f;
		float m_rotationLimitsMaxPitch = 0.0f;
	};
}

