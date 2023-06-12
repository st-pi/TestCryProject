// Copyright 2017-2019 Crytek GmbH / Crytek Group. All rights reserved.
#pragma once

////////////////////////////////////////////////////////
// Represents a player participating in gameplay
////////////////////////////////////////////////////////

namespace Cry::DefaultComponents
{
	class CCameraComponent;
	class CInputComponent;
	class CCharacterControllerComponent;
	class CAdvancedAnimationComponent;
}
namespace Game
{
	class CWeaponComponent;
	class CPlayerComponent final : public IEntityComponent
	{
	public:
		enum class EInputFlagType
		{
			Hold = 0,
			Toggle,
			Tap
		};

		enum class EInputFlag : uint8
		{
			MoveForward = 1 << 0,
			MoveBack = 1 << 1,
			MoveRight = 1 << 2,
			MoveLeft = 1 << 3,
			Walk = 1 << 4,
			Jump = 1 << 5
		};


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
			desc.AddMember(&CPlayerComponent::m_walkSpeed, 'pws', "playerwalkspeed", "Player Walking Movement Speed", "Sets the Player walking movement speed", ZERO);
			desc.AddMember(&CPlayerComponent::m_rotationSpeed, 'prs', "playerrotationspeed", "Player Rotation Speed", "Sets the speed of the players rotationPlayer", ZERO);
			desc.AddMember(&CPlayerComponent::m_cameraDefaultPos, 'cdp', "cameradefaultpos", "Camera Default Position", "Sets the camera position", ZERO);
			desc.AddMember(&CPlayerComponent::m_rotationLimitsMinPitch, 'cpmi', "camerapitchmin", "Camera Pitch Min", "Minimum rotation value for camera pitch", -0.85f);
			desc.AddMember(&CPlayerComponent::m_rotationLimitsMaxPitch, 'cpm', "camerapitchmax", "Camera Pitch Max", "Maximum rotation value for camera pitch", 1.5f);
			desc.AddMember(&CPlayerComponent::m_JumpHeight, 'pjm', "playerjumpheight", "Player Jump Height", "Sets the Player jump height", 5.0f);
		}

	protected:

	private:
		void InitializeInput();
		void UpdatePlayerMovement(float deltaTime);
		void UpdateCameraRotation();
		void HandleInputFlagChange(const EInputFlag inputFlags, const EInputFlagType inputType, int activationMode);
		bool IsInputFlagActive(const EInputFlag inputFlag) const;

	private:
		Cry::DefaultComponents::CCameraComponent* m_pCameraComponent = nullptr;
		Cry::DefaultComponents::CInputComponent* m_pInputComponent = nullptr;
		Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponnet = nullptr;
		Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent = nullptr;
		CWeaponComponent* m_pWeaponComponent = nullptr;

		Quat m_lookOrientation;

		Vec3 m_cameraDefaultPos;

		Vec2 m_mouseDeltaRotation;

		float m_walkSpeed = 0.0f;
		float m_movementSpeed = 0.0f;
		float m_rotationSpeed = 0.0f;
		float m_JumpHeight = 0.0f;

		float m_rotationLimitsMinPitch = 0.0f;
		float m_rotationLimitsMaxPitch = 0.0f;

		CEnumFlags<EInputFlag> m_inputFlags;
	};
}

