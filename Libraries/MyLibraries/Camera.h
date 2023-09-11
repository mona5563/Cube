//
// Camera.h
//
#pragma once

#include <SimpleMath.h>

/// <summary>
/// �f�o�b�O�p�J�����N���X
/// </summary>
class Camera
{
public:
	static const float DEFAULT_CAMERA_DISTANCE;	// �J�����̋���

private:

	// �r���[�s��
	DirectX::SimpleMath::Matrix m_view;

	// �v���W�F�N�V�����s��
	DirectX::SimpleMath::Matrix m_projection;

	// �J�������W
	DirectX::SimpleMath::Vector3 m_eye;

	// �����_
	DirectX::SimpleMath::Vector3 m_target;

	// ������x�N�g��
	DirectX::SimpleMath::Vector3 m_up;

public:
	// �R���X�g���N�^
	Camera();

	// �f�X�g���N�^
	~Camera();

	// �r���[�s��̃A�N�Z�T
	void SetViewMatrix(DirectX::SimpleMath::Matrix view) { m_view = view; }
	DirectX::SimpleMath::Matrix GetViewMatrix() const { return m_view; }

	// �v���W�F�N�V�����s��̃A�N�Z�T
	void SetProjectionMatrix(DirectX::SimpleMath::Matrix projection) { m_projection = projection; }
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const { return m_projection; }

	// �J�������W�̃A�N�Z�T
	void SetEyePosition(DirectX::SimpleMath::Vector3 eye) { m_eye = eye; }
	DirectX::SimpleMath::Vector3 GetEyePosition() const { return m_eye; }

	// �����_���W�̃A�N�Z�T
	void SetTargetPosition(DirectX::SimpleMath::Vector3 target) { m_target = target; }
	DirectX::SimpleMath::Vector3 GetTargetPosition() const { return m_target; }

	// ������x�N�g���̃A�N�Z�T
	void SetUpVector(DirectX::SimpleMath::Vector3 up) { m_up = up; }
	DirectX::SimpleMath::Vector3 GetUpVector() const { return m_up; }

protected:

	// �r���[�s��̎Z�o
	virtual void CalculateViewMatrix();

	// �v���W�F�N�V�����s��̎Z�o
	virtual void CalculateProjectionMatrix();
};
