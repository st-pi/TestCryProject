// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Player.h"

#include <DefaultComponents/Cameras/CameraComponent.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Geometry/AdvancedAnimationComponent.h>
#include "Weapon.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

namespace Game
{
	static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
	{
		Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
		{
			Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
		}
	}

	CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);

	void CPlayerComponent::Initialize()
	{
		m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
		m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();
		m_pCharacterControllerComponnet = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCharacterControllerComponent>();
		m_pAdvancedAnimationComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CAdvancedAnimationComponent>();
		m_pWeaponComponent = m_pEntity->GetOrCreateComponent<CWeaponComponent>();
	}

	Cry::Entity::EventFlags CPlayerComponent::GetEventMask() const
	{
		return Cry::Entity::EEvent::GameplayStarted | Cry::Entity::EEvent::Update | Cry::Entity::EEvent::Reset;
	}

	void CPlayerComponent::ProcessEvent(const SEntityEvent& event)
	{
		switch (event.event)
		{
		case Cry::Entity::EEvent::GameplayStarted:
		{
			InitializeInput();
			break;
		}
		case Cry::Entity::EEvent::Update:
		{
			UpdatePlayerMovement();
			UpdateCameraRotation();

			break;
		}
		case Cry::Entity::EEvent::Reset:
		{
			m_movementDelta = ZERO;
			m_mouseDeltaRotation = ZERO;
			m_lookOrientation = IDENTITY;

			Matrix34 cameraDefaultMatrix;
			cameraDefaultMatrix.SetTranslation(m_cameraDefaultPos);
			cameraDefaultMatrix.SetRotation33(Matrix33(m_pEntity->GetWorldRotation()));
			m_pCameraComponent->SetTransformMatrix(cameraDefaultMatrix);
			break;
		}
		}
	}

	void CPlayerComponent::InitializeInput()
	{
		m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) {m_movementDelta.y = value; });
		m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);

		m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) {m_movementDelta.y = -value; });
		m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, eKI_S);

		m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) {m_movementDelta.x = value; });
		m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);

		m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) {m_movementDelta.x = -value; });
		m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);

		m_pInputComponent->RegisterAction("player", "yaw", [this](int activationMode, float value) {m_mouseDeltaRotation.y = -value; });
		m_pInputComponent->BindAction("player", "yaw", eAID_KeyboardMouse, eKI_MouseY);

		m_pInputComponent->RegisterAction("player", "pitch", [this](int activationMode, float value) {m_mouseDeltaRotation.x = -value; });
		m_pInputComponent->BindAction("player", "pitch", eAID_KeyboardMouse, eKI_MouseX);

		m_pInputComponent->RegisterAction("player", "walk", [this](int activationMode, float value) {m_isWalkPressed = (activationMode == eAAM_OnPress || activationMode == eAAM_OnHold); });
		m_pInputComponent->BindAction("player", "walk", eAID_KeyboardMouse, eKI_LShift);

		m_pInputComponent->RegisterAction("player", "jump", [this](int activationMode, float value) {m_isJumpPressed = activationMode == eAAM_OnPress; });
		m_pInputComponent->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);

		m_pInputComponent->RegisterAction("player", "shoot", [this](int activationMode, float value) 
			{
				if (activationMode == eAAM_OnPress)
				{
					if (auto pCharacter = m_pAdvancedAnimationComponent->GetCharacter())
					{
						if (auto pBarrelAttachment = pCharacter->GetIAttachmentManager()->GetInterfaceByName("barrel_out"))
						{
							m_pWeaponComponent->Shoot(pBarrelAttachment->GetAttWorldAbsolute());
						}
					}
				}
			});
		m_pInputComponent->BindAction("player", "shoot", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);
	}

	void CPlayerComponent::UpdatePlayerMovement()
	{
		Vec3 velocity = Vec3(m_movementDelta.x, m_movementDelta.y, 0.0f);
		velocity.Normalize();
		velocity *= (m_isWalkPressed ? m_walkSpeed : m_movementSpeed);

		if (m_pCharacterControllerComponnet->IsOnGround() && m_isJumpPressed)
		{
			Vec3 jumpVelocity(0, 0, m_JumpHeight);
			m_pCharacterControllerComponnet->ChangeVelocity(jumpVelocity, Cry::DefaultComponents::CCharacterControllerComponent::EChangeVelocityMode::Add);
		}

		m_pCharacterControllerComponnet->SetVelocity(m_pEntity->GetWorldRotation() * velocity);
	}

	void CPlayerComponent::UpdateCameraRotation()
	{
		Ang3 rotationAngle = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
		rotationAngle.x += m_mouseDeltaRotation.x * m_rotationSpeed;
		rotationAngle.y = CLAMP(rotationAngle.y + m_mouseDeltaRotation.y * m_rotationSpeed, m_rotationLimitsMinPitch, m_rotationLimitsMaxPitch);
		rotationAngle.z = 0;

		m_lookOrientation = Quat(CCamera::CreateOrientationYPR(rotationAngle));

		Ang3 yawAngle = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
		yawAngle.y = 0;
		const Quat finalYaw = Quat(CCamera::CreateOrientationYPR(yawAngle));
		m_pEntity->SetRotation(finalYaw);

		Ang3 pitchAngle = CCamera::CreateAnglesYPR(Matrix33(m_lookOrientation));
		pitchAngle.x = 0;
		Matrix34 finalCamMatrix;
		finalCamMatrix.SetTranslation(m_pCameraComponent->GetTransformMatrix().GetTranslation());
		Matrix33 camRotation = CCamera::CreateOrientationYPR(pitchAngle);
		finalCamMatrix.SetRotation33(camRotation);
		m_pCameraComponent->SetTransformMatrix(finalCamMatrix);
	}
}


