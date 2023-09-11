//
//Box.cpp
//
#include "pch.h"
#include "Box.h"

#include "Game/ResourceManager.h"


//��������
const float Box::MOVE_TIME = 1.0f;


//============================================================================
// �R���X�g���N�^
//============================================================================

Box::Box()
	:
	m_pBox(nullptr),
	m_color(DirectX::Colors::White),
	m_hit(false),
	m_moveTime(0.0f),
	m_time(0.0f),
	m_state(State::STAY),
	m_type(Type::NORMAL),
	m_boxRoutine(0),
	m_renderPos(DirectX::SimpleMath::Vector3::Zero)
{	
	//�J�v�Z���̔��a��0.55�Ƃ���
	m_hitCapsule.radius = 0.55f;
}

//============================================================================
// �f�X�g���N�^
//============================================================================
Box::~Box()
{
	Finalize();
}

//============================================================================
// ����������
//============================================================================
void Box::Initialize(const DirectX::SimpleMath::Vector3& cupsulePos, Type type)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//ResourceManager�̃C���X�^���X���擾
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//�O�g�̃e�N�X�`���̎擾
	m_frame = resourceManager.GetTexture(eTextureID::BOX_FRAME);
	m_clearframe_1 = resourceManager.GetTexture(eTextureID::CLEARBOX_FRAME_1);
	m_clearframe_2 = resourceManager.GetTexture(eTextureID::CLEARBOX_FRAME_2);

	//�W�I���g���b�N�v���~�e�B�u�̍쐬
	m_pBox = DirectX::GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());

	//�}�E�X���C�̓����蔻��̏�����
	m_hit = false;

	//	�J�v�Z���̏㑤���n�_�Ƃ���
	m_hitCapsule.bigin.x = cupsulePos.x + 0.15f;
	m_hitCapsule.bigin.y = cupsulePos.y + 0.15f;
	m_hitCapsule.bigin.z = cupsulePos.z + 0.15f;

	//	�J�v�Z���̉������I�_�Ƃ���
	m_hitCapsule.end.x = cupsulePos.x - 0.15f;
	m_hitCapsule.end.y = cupsulePos.y - 0.15f;
	m_hitCapsule.end.z = cupsulePos.z - 0.15f;

	//�^�C�v�̐ݒ�
	m_type = type;
}

//============================================================================
// �X�V����
//============================================================================
void Box::Update(const DX::StepTimer& timer, float scalingRatio)
{
	//�F���擾����
	m_color = GetColor();
	//�ړ��I���ꏊ�̎擾������
	m_goalPos = GetGoalPos();

	if (m_state == State::MOVING)
	{
		//�o�ߎ��Ԃ��擾����
		m_time = (float)timer.GetElapsedSeconds();
		//�ړ����Ԃɔ��f����
		m_moveTime += m_time;
		//�ړ�������
		if (m_moveTime > MOVE_TIME)
		{
			//�ړ��J�n�ʒu���ړ���̍��W�Ɏw�肷��
			m_startPos = m_position;
			//���W���ړ���̈ʒu�Ɏw�肷��
			m_position = m_goalPos;

			//����p�J�v�Z�����ړ�������
			//�J�v�Z���̏㑤���n�_�Ƃ���
			m_hitCapsule.bigin.x = (m_position.x + 0.15f) * scalingRatio;
			m_hitCapsule.bigin.y = (m_position.y + 0.15f) * scalingRatio;
			m_hitCapsule.bigin.z = (m_position.z + 0.15f) * scalingRatio;
			//�J�v�Z���̉������I�_�Ƃ���
			m_hitCapsule.end.x = (m_position.x - 0.15f) * scalingRatio;
			m_hitCapsule.end.y = (m_position.y - 0.15f) * scalingRatio;
			m_hitCapsule.end.z = (m_position.z - 0.15f) * scalingRatio;

			//�ړ��������������߁A��Ԃ�҂��ɂ���
			m_state = State::STAY;
			//�ړ����Ԃ����Z�b�g����
			m_moveTime = 0.0f;
		}
	}
}

//============================================================================
// �`�悷��
//============================================================================
void Box::Render(const Camera* pCamera)
{
	//���O���
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//�ϊ��s��
	Matrix world = Matrix::Identity;		//���[���h�s��
	Matrix view = pCamera->GetViewMatrix();				//�r���[�s��
	Matrix projection = pCamera->GetProjectionMatrix();	//�ˉe�s��
	
	//�C�[�W���O�֐����g�p���A�{�b�N�X���ړ�����悤�`�悷��
	m_renderPos = Vector3::Lerp(
		m_position,
		m_goalPos,
		easeInBounce(m_moveTime / MOVE_TIME)
	);
	world = Matrix::CreateTranslation(m_renderPos);

	//�{�b�N�X�̕`��(�^�C�v�ɂ���ăt���[���摜��ς���)
	switch (m_type)
	{
	case Box::Type::NORMAL:
		m_pBox->Draw(
			world,
			view,
			projection,
			m_color,
			m_frame.Get()
		);
		break;
	case Box::Type::CLEAR_1:
		m_pBox->Draw(
			world, 
			view,
			projection,
			m_color,
			m_clearframe_1.Get()
		);
		break;
	case Box::Type::CLEAR_2:
		m_pBox->Draw(
			world,
			view,
			projection,
			m_color,
			m_clearframe_2.Get()
		);
		break;
	}
	
}



//============================================================================
//��n������
//============================================================================
void Box::Finalize()
{
}

//============================================================================
//�ʒu���擾����
//============================================================================
const DirectX::SimpleMath::Vector3& Box::GetPosition() const
{
	return m_position;
}

//============================================================================
//�ʒu��ݒ肷��
//============================================================================
void Box::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

//============================================================================
//�F���擾����
//============================================================================
const DirectX::SimpleMath::Color& Box::GetColor() const
{
	return m_color;
}

//============================================================================
//�F��ݒ肷��
//============================================================================
void Box::SetColor(const DirectX::SimpleMath::Color& color)
{
	m_color = color;
}

//============================================================================
//�ړ��J�n�ʒu���擾����
//============================================================================
DirectX::SimpleMath::Vector3 Box::GetStartPos() const
{
	return m_startPos;
}
//============================================================================
//�ړ��J�n�ʒu��ݒ肷��
//============================================================================
void Box::SetStartPos(const DirectX::SimpleMath::Vector3& startPos)
{
	m_startPos = startPos;
}
//============================================================================
//�ړ���ʒu���擾����
//============================================================================
DirectX::SimpleMath::Vector3 Box::GetGoalPos() const
{
	return m_goalPos;
}
//============================================================================
//�ړ���ʒu��ݒ肷��
//============================================================================
void Box::SetGoalPos(const DirectX::SimpleMath::Vector3& goalPos)
{
	m_goalPos = goalPos;
}

//============================================================================
//���`���
//============================================================================
float Box::Lerp(const float& start, const float& end, const float& t)
{
	return start + (end - start) * t;
}

//============================================================================
//�C�[�W���O�֐�
//============================================================================
float Box::easeOutBounce(const float& x)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	float x1 = x;

	if (x1 < 1.0f / d1) {
		return n1 * x1 * x1;
	}
	else if (x1 < 2.0f / d1) {
		return n1 * (x1 -= 1.5f / d1) * x1+ 0.75f;
	}
	else if (x1 < 2.5f / d1) {
		return n1 * (x1 -= 2.25f / d1) * x1 + 0.9375f;
	}
	else {
		return n1 * (x1 -= 2.625f / d1) * x1 + 0.984375f;
	}
}

float Box::easeInBounce(const float& x)
{
	return 1.0f - easeOutBounce(1.0f - x);
}

////============================================================================
////�����蔻��
////============================================================================
//bool Box::HitCheck_Capsule2Capsule(const Capsule& mouse)
//{
//	float s, t;
//	DirectX::SimpleMath::Vector3 c1, c2;
//	//	�J�v�Z���̒��S�̐����Ԃ̋����̕������v�Z
//	//	s, t, c1, c2�͖߂�l
//	float dist2 = ClosestPtSegmentSegment(mouse.bigin, mouse.end, s, t, c1, c2);
//	float radius = m_hitCapsule.radius + mouse.radius;
//	return dist2 <= radius * radius;
//}
//
////============================================================================
////�͈͐���
////============================================================================
//float Box::Clamp(const float& n, const float& min, const float& max)
//{
//	if (n < min) return min;
//	if (n > max) return max;
//	return n;
//}
//
////============================================================================
////�Q�̐����̍ŒZ�����̕�����Ԃ��֐�
////============================================================================
//float Box::ClosestPtSegmentSegment(
//	const DirectX::SimpleMath::Vector3& mouseBigin, const DirectX::SimpleMath::Vector3& mouseEnd,
//	float& s, float& t, DirectX::SimpleMath::Vector3& c1, DirectX::SimpleMath::Vector3& c2)
//{
//	//	p1��q1�̃x�N�g�����Z�o
//	DirectX::SimpleMath::Vector3 biginVec = m_hitCapsule.end - m_hitCapsule.bigin;
//	//	p2��q2�̃x�N�g�����Z�o
//	DirectX::SimpleMath::Vector3 endVec = mouseEnd - mouseBigin;
//	//	��̃J�v�Z�����������́A�n�_�Ԃ̃x�N�g���𔼌a�̍��v�Ƃ��ĎZ�o
//	DirectX::SimpleMath::Vector3 r = m_hitCapsule.bigin - mouseBigin;
//
//	//	p1��q1�̃x�N�g���̒�����2����Z�o
//	float a = biginVec.Dot(biginVec);
//	//	p2��q2�̃x�N�g���̒�����2����Z�o
//	float e = endVec.Dot(endVec);
//	//	p1��q1�̃x�N�g���Ɣ��a�̍��v�ƂȂ�x�N�g������ς���
//	float f = endVec.Dot(r);
//
//	if (a <= FLT_EPSILON && e <= FLT_EPSILON)
//	{
//		//	a��e�̗������ق�0��������A��̃J�v�Z���͕��s�ł���
//		s = t = 0.0f;
//
//		//	���Ƃ��Ċe�n�_���l������
//		c1 = m_hitCapsule.bigin;
//		c2 = mouseBigin;
//		//	���擾�����_���m�̋��������߂�
//		return (c1 - c2).Dot(c1 - c2);
//	}
//	if (a <= FLT_EPSILON)
//	{
//		//	a���ق�0�������ꍇ�́Af/e��t�Ƃ��Čv�Z����
//		//	�������A0�`1�̊Ԃɕ␳����
//		s = 0.0f;
//		t = f / e;
//		t = Clamp(t, 0.0f, 1.0f);
//	}
//	else
//	{
//		//	p1��q1�̃x�N�g���Ɣ��a�̍��v����ς���
//		float c = biginVec.Dot(r);
//		if (e <= FLT_EPSILON)
//		{
//			//	e���ق�0�������ꍇ�́As�Ƃ���-c/a���v�Z����B
//			//	�������A0�`1�̊Ԃɕ␳����
//			t = 0.0f;
//			s = Clamp(-c / a, 0.0f, 1.0f);
//		}
//		else
//		{
//			//	����0���Z���N����댯�͂Ȃ��̂ŁA���ʂ�̌v�Z���s��
//			//	�e�J�v�Z���̃x�N�g���œ��ς���
//			float b = biginVec.Dot(endVec);
//			//	���ꂼ��̃J�v�Z���̒�����2����|�����l�ƁA
//			//	�e�J�v�Z���̃x�N�g���œ��ς������ʂ�2����ׂ�
//			float denom = a * e - b * b;
//			if (denom != 0.0f)
//			{
//				//	����������Ă���΁A���ʂ�̌v�Z���s��
//				s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
//			}
//			else
//			{
//				//	��̃J�v�Z���̌������ق�90�x�̏ꍇ�As��0�Ƃ���
//				s = 0.0f;
//			}
//
//			//	b * s + f ���v�Z���Č��؂���
//			float tnom = (b * s + f);
//			if (tnom < 0.0f)
//			{
//				//	�}�C�i�X�̒l�ɂȂ��Ă�����A-c / a�̒l��s�Ƃ���
//				t = 0.0f;
//				s = Clamp(-c / a, 0.0f, 1.0f);
//			}
//			else if (tnom > e)
//			{
//				//	e���傫����΁A(b - c) / a�̒l��s�Ƃ���
//				t = 1.0f;
//				s = Clamp((b - c) / a, 0.0f, 1.0f);
//			}
//			else
//			{
//				//	���ʂ�̌v�Z���s��
//				t = tnom / e;
//			}
//		}
//	}
//
//	//	���ꂼ��̃J�v�Z���ɂ�����A��ԋ߂��_���Z�o����
//	c1 = m_hitCapsule.bigin + biginVec * s;
//	c2 = mouseBigin + endVec * t;
//
//	//	��ԋ߂���_�̋����̓���Ԃ�
//	return (c1 - c2).Dot(c1 - c2);
//}
//
