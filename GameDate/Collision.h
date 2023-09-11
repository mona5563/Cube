#pragma once

#include "GameDate/Objects.h"

// �J�v�Z���̔���p�\����
class CapsuleCollider
{
public:
	//����p�J�v�Z����ݒ肷��
	void SetCapsule(Capsule capsule);

public:
	//�R���X�g���N�^
	CapsuleCollider();
	//�f�X�g���N�^
	~CapsuleCollider();

	//���̃J�v�Z���Ƃ̓����蔻�������
	bool CheckHit(const Capsule& collider);
	//�͈͂̐��������
	float Clamp(
		const float& n,
		const float& min,
		const float& max
	);
	//2�̐����̍ŒZ�����̕�����Ԃ�
	float ClosestPtSegmentSegment(
		const Capsule& collider,
		const DirectX::SimpleMath::Vector3& bigin,
		const DirectX::SimpleMath::Vector3& end,
		float& s,
		float& t,
		DirectX::SimpleMath::Vector3& c1,
		DirectX::SimpleMath::Vector3& c2
	);
private:
	//����p�J�v�Z��
	Capsule m_capsule;
};