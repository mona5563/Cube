#include "pch.h"
#include "Collision.h"

/*--------------------------------------------------
����p�J�v�Z����ݒ肷��(�{�b�N�X�̃J�v�Z��)
--------------------------------------------------*/
void CapsuleCollider::SetCapsule(Capsule capsule)
{
	m_capsule = capsule;
}

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
CapsuleCollider::CapsuleCollider()
	:
	m_capsule{}
{
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
CapsuleCollider::~CapsuleCollider()
{
}

/*--------------------------------------------------
���̃J�v�Z���Ƃ̓����蔻�������
--------------------------------------------------*/
bool CapsuleCollider::CheckHit(const Capsule& collider)
{
	float s, t;
	DirectX::SimpleMath::Vector3 c1, c2;
	//	�J�v�Z���̒��S�̐����Ԃ̋����̕������v�Z
	//	s, t, c1, c2�͖߂�l
	float dist2 = ClosestPtSegmentSegment(collider, m_capsule.bigin, m_capsule.end, s, t, c1, c2);
	float radius = collider.radius + m_capsule.radius;
	return dist2 <= radius * radius;
}

/*--------------------------------------------------
�͈͂̐��������
--------------------------------------------------*/
float CapsuleCollider::Clamp(
	const float& n,
	const float& min,
	const float& max
)
{
	if (n < min) return min;
	if (n > max) return max;
	return n;
}

/*--------------------------------------------------
//2�̐����̍ŒZ�����̕�����Ԃ�
--------------------------------------------------*/
float CapsuleCollider::ClosestPtSegmentSegment(
	const Capsule& collider,
	const DirectX::SimpleMath::Vector3& bigin,
	const DirectX::SimpleMath::Vector3& end,
	float& s,
	float& t,
	DirectX::SimpleMath::Vector3& c1,
	DirectX::SimpleMath::Vector3& c2
)
{
	//	p1��q1�̃x�N�g�����Z�o
	DirectX::SimpleMath::Vector3 biginVec = collider.end - collider.bigin;
	//	p2��q2�̃x�N�g�����Z�o
	DirectX::SimpleMath::Vector3 endVec = end - bigin;
	//	��̃J�v�Z�����������́A�n�_�Ԃ̃x�N�g���𔼌a�̍��v�Ƃ��ĎZ�o
	DirectX::SimpleMath::Vector3 r = collider.bigin - bigin;

	//	p1��q1�̃x�N�g���̒�����2����Z�o
	float a = biginVec.Dot(biginVec);
	//	p2��q2�̃x�N�g���̒�����2����Z�o
	float e = endVec.Dot(endVec);
	//	p1��q1�̃x�N�g���Ɣ��a�̍��v�ƂȂ�x�N�g������ς���
	float f = endVec.Dot(r);

	if (a <= FLT_EPSILON && e <= FLT_EPSILON)
	{
		//	a��e�̗������ق�0��������A��̃J�v�Z���͕��s�ł���
		s = t = 0.0f;

		//	���Ƃ��Ċe�n�_���l������
		c1 = collider.bigin;
		c2 = bigin;
		//	���擾�����_���m�̋��������߂�
		return (c1 - c2).Dot(c1 - c2);
	}
	if (a <= FLT_EPSILON)
	{
		//	a���ق�0�������ꍇ�́Af/e��t�Ƃ��Čv�Z����
		//	�������A0�`1�̊Ԃɕ␳����
		s = 0.0f;
		t = f / e;
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		//	p1��q1�̃x�N�g���Ɣ��a�̍��v����ς���
		float c = biginVec.Dot(r);
		if (e <= FLT_EPSILON)
		{
			//	e���ق�0�������ꍇ�́As�Ƃ���-c/a���v�Z����B
			//	�������A0�`1�̊Ԃɕ␳����
			t = 0.0f;
			s = Clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			//	����0���Z���N����댯�͂Ȃ��̂ŁA���ʂ�̌v�Z���s��
			//	�e�J�v�Z���̃x�N�g���œ��ς���
			float b = biginVec.Dot(endVec);
			//	���ꂼ��̃J�v�Z���̒�����2����|�����l�ƁA
			//	�e�J�v�Z���̃x�N�g���œ��ς������ʂ�2����ׂ�
			float denom = a * e - b * b;
			if (denom != 0.0f)
			{
				//	����������Ă���΁A���ʂ�̌v�Z���s��
				s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
			{
				//	��̃J�v�Z���̌������ق�90�x�̏ꍇ�As��0�Ƃ���
				s = 0.0f;
			}

			//	b * s + f ���v�Z���Č��؂���
			float tnom = (b * s + f);
			if (tnom < 0.0f)
			{
				//	�}�C�i�X�̒l�ɂȂ��Ă�����A-c / a�̒l��s�Ƃ���
				t = 0.0f;
				s = Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (tnom > e)
			{
				//	e���傫����΁A(b - c) / a�̒l��s�Ƃ���
				t = 1.0f;
				s = Clamp((b - c) / a, 0.0f, 1.0f);
			}
			else
			{
				//	���ʂ�̌v�Z���s��
				t = tnom / e;
			}
		}
	}

	//	���ꂼ��̃J�v�Z���ɂ�����A��ԋ߂��_���Z�o����
	c1 = collider.bigin + biginVec * s;
	c2 = bigin + endVec * t;

	//	��ԋ߂���_�̋����̓���Ԃ�
	return (c1 - c2).Dot(c1 - c2);
}
