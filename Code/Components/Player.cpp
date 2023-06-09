// Copyright 2017-2020 Crytek GmbH / Crytek Group. All rights reserved.
#include "StdAfx.h"
#include "Player.h"
#include "GamePlugin.h"
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
	}

	void CPlayerComponent::UpdatePlayerMovement()
	{
		Vec3 velocity = Vec3(m_movementDelta.x, m_movementDelta.y, 0.0f);
		velocity.Normalize();
		m_pCharacterControllerComponnet->SetVelocity(m_pEntity->GetWorldRotation() * velocity * m_movementSpeed);
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


