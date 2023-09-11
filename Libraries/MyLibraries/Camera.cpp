//
// Camera.cpp
//
#include "pch.h"
#include "Camera.h"

#include "DeviceResources.h"


using namespace DirectX;

// �萔�̏�����
const float Camera::DEFAULT_CAMERA_DISTANCE = 5.0f;

//-------------------------------------------------------------------
// �R���X�g���N�^
//-------------------------------------------------------------------
Camera::Camera()
	: m_view(DirectX::SimpleMath::Matrix::Identity)
	, m_eye(0.0f, 0.0f, 10.0f)
	, m_target(0.0f, 0.0f, 0.0f)
	, m_up(0.0f, 1.0f, 0.0f)
{
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}


//-------------------------------------------------------------------
// �f�X�g���N�^
//-------------------------------------------------------------------
Camera::~Camera()
{
}

//-------------------------------------------------------------------
// �r���[�s��̎Z�o
//-------------------------------------------------------------------
void Camera::CalculateViewMatrix()
{
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_eye, m_target, m_up);
}

//-------------------------------------------------------------------
// �v���W�F�N�V�����s��̎Z�o
//-------------------------------------------------------------------
void Camera::CalculateProjectionMatrix()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// �E�B���h�E�T�C�Y�̎擾
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	constexpr float fieldOfView = XMConvertToRadians(45.0f);    // ��p
	float aspectRatio = width / height;							// ��ʏc����
	float nearPlane = 1.0f;                                     // �J���������ԋ߂����e��
	float farPlane = 100.0f;                                    // �J���������ԉ������e��
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);
}
