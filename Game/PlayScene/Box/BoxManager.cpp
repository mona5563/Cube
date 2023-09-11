#include "pch.h"
#include "BoxManager.h"

#include "Game/ResourceManager.h"



BoxManager::BoxManager()
	:m_pBox{}
	, m_color(DirectX::Colors::White)
	, m_movingBox(nullptr)
	, m_nullPos(DirectX::SimpleMath::Vector3::Zero)
	, m_state(State::STAY)
	, m_pGoalBox1(nullptr)
	, m_pGoalBox2(nullptr)
	, m_pShadowBox(nullptr)
	, m_changeScene(false)
	, m_isRender(false)
	, m_isVicinityNull(false)
	, m_isVicinityClear(false)
	, m_moveCount(0)
	, m_soundID(SoundID::NONE)
{
}

BoxManager::~BoxManager()
{
}

void BoxManager::Initialize(const int emptyBoxIndex, const int goalBoxIndex_1, const int goalBoxIndex_2, const int clearBoxIndex_1, const int clearBoxIndex_2)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//�K�v�ȕϐ��̏���
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	//TODO::PlayScene�ō쐬(?)
	//�S�[���ʒu��������{�b�N�X�̍쐬
	m_pGoalBox1 = GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());
	m_pGoalBox2 = GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());
	//ResourceManager�̃C���X�^���X���擾
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//�e�摜�̎擾
	m_goalframe_1 = resourceManager.GetTexture(eTextureID::GOALBOX_FRAME_1);
	m_goalframe_2 = resourceManager.GetTexture(eTextureID::GOALBOX_FRAME_2);
	//�e�p�{�b�N�X
	m_pShadowBox = GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());

	//�{�b�N�X�̐����Ə�����
	m_pBox.resize(3 * 3 * 3); //3*3*3�̃{�b�N�X�̔z��̃T�C�Y
	for (auto& box : m_pBox)
	{
		box = std::make_unique<Box>();
	}
	//1������ɂ���
	Delete(emptyBoxIndex);

	//�{�b�N�X�̈ʒu
	float posX = -1.05f;
	float posY = -1.05f;
	float posZ = -1.05f;

	//�e�{�b�N�X�̏����ʒu�ƃ^�C�v�̐ݒ�
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			if (box.get() == m_pBox[clearBoxIndex_1].get())
			{
				box->Initialize(SimpleMath::Vector3(posX, posY, posZ), Box::Type::CLEAR_1);
				m_clearBox1Pos = box->GetPosition();
			}
			else if (box.get() == m_pBox[clearBoxIndex_2].get())
			{
				box->Initialize(Vector3(posX, posY, posZ), Box::Type::CLEAR_2);
				m_clearBox2Pos = box->GetPosition();
			}
			else
			{
				box->Initialize(Vector3(posX, posY, posZ), Box::Type::NORMAL);
			}

			box->SetPosition(Vector3(posX, posY, posZ));
		}
		else
		{
			//�󂢂Ă���ʒu
			m_nullPos = Vector3(posX, posY, posZ);
		}

		//�{�b�N�X��3�~3�~3�ɂȂ�悤�ɒ���
		if (posX < 1.0f)
		{
			posX += 1.05f;
		}
		else if (posY < 1.0f)
		{
			posX = -1.05f;
			posY += 1.05f;
		}
		else
		{
			posX = -1.05f;
			posY = -1.05f;
			posZ += 1.05f;
		}
	}
	//�S�[���ʒu�̐ݒ�
	m_goalBox1Pos = m_pBox[goalBoxIndex_1]->GetPosition();
	m_goalBox2Pos = m_pBox[goalBoxIndex_2]->GetPosition();
	//�G�t�F�N�g�̐���
	m_boxEffect = std::make_unique<EffectManager>();
	m_boxEffect->Create();

	//�R���C�_�[�̐���
	m_collider = std::make_unique<CapsuleCollider>();
}

void BoxManager::Update(const DX::StepTimer& timer, DirectX::Mouse::ButtonStateTracker mouseStateTracker, Capsule mouseRay, DebugCamera* camera, float scalingRatio)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//�J�����Ƃ̉��̍ő勗��
	float length = 999.9f;
	//�������Ă���{�b�N�X�̕ۑ���
	Box* pHitBox = nullptr;

	switch (m_state)
	{
	//�{�b�N�X�̏�Ԃ������Ă��Ȃ�������
	case State::STAY:
	{
		m_soundID = SoundID::NONE;
		for (auto& box : m_pBox)
		{
			if (box == nullptr)
			{
				continue;
			}

			//�{�b�N�X����J�v�Z�����󂯎���ăR���C�_�[�N���X���瓖���蔻�������
			m_collider->SetCapsule(box->GetCapsule());
			bool isHit = m_collider->CheckHit(mouseRay);

			//�}�E�X�Ƃ̓����蔻�������
			//bool isHit = box->HitCheck_Capsule2Capsule(mouseRay);
			
			//�}�E�X���������Ă�����
			if (isHit)
			{
				//�J�����Ƃ̋������r����
				float dist = DirectX::SimpleMath::Vector3::Distance(camera->GetEyePosition(), box->GetPosition());
				if (dist < length)
				{
					//�J�����ɋ߂����̂𓖂��������̂Ɣ��肷��
					pHitBox = box.get();
					length = dist;
				}
			}
		}

		for (auto& box : m_pBox)
		{
			if (box == nullptr)
			{
				continue;
			}

			//�J�����ƈ�ԋ߂������̃{�b�N�X��������
			if (box.get() == pHitBox)
			{
				//�F��ς���
				m_color = Colors::LightCoral;
			}
			else
			{
				//�}�E�X�ɓ������Ă��Ȃ����̐F���w�肷��
				if (box->GetType() == Box::Type::CLEAR_1)
				{
					//�N���A�����̃{�b�N�X�̓I�����W�F�ɐݒ�
					m_color = Colors::Orange;
				}
				else if (box->GetType() == Box::Type::CLEAR_2)
				{
					//�N���A����2�ڂ̃{�b�N�X�̓��C���F�ɐݒ�
					m_color = Colors::Lime;
				}
				else
				{
					//���̑��̃{�b�N�X�͔��F�ɐݒ�
					m_color = Colors::White;
				}
			}

			//�{�b�N�X�̐F�̐ݒ�
			box->SetColor(m_color);
		}

		//�J�����ƈ�ԋ߂������̃{�b�N�X��������
		if (pHitBox != nullptr)
		{
			//�N���b�N���ꂽ��{�b�N�X�𓮂���
			if (mouseStateTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED)
			{
				//�{�b�N�X�ׂ̗��󂢂Ă��邩����
				m_isVicinityNull = VicinityNullPos(m_nullPos, pHitBox->GetPosition());

				//�ׂ��󂢂Ă�����{�b�N�X�𓮂�������������
				if (m_isVicinityNull)
				{
					//�ړ��񐔂��v�Z����
					m_moveCount++;
					//�������{�b�N�X���w�肷��
					m_movingBox = pHitBox;
					//�ړ��̃S�[���ʒu�̐ݒ�
					m_movingBox->SetGoalPos(m_nullPos);
					//�X�e�[�g��MOVING�ɕύX
					m_movingBox->SetCurrentState(Box::State::MOVING);

					//�����ړ������{�b�N�X���N���A�����̃{�b�N�X��������
					if (pHitBox->GetType() == Box::Type::CLEAR_1)
					{
						//�N���A�����̃{�b�N�X�̈ʒu���擾����
						m_isVicinityClear = VicinityClearPos(m_clearBox1Pos, m_goalBox1Pos);
					}
					else if (pHitBox->GetType() == Box::Type::CLEAR_2)
					{
						//�N���A�����̃{�b�N�X�̈ʒu���擾����
						m_isVicinityClear = VicinityClearPos(m_clearBox2Pos, m_goalBox2Pos);
					}
					//�{�b�N�X�𓮂�����Ԃɂ���
					m_state = State::BOX_MOVING;
				}
				else
				{
					//�����Ȃ��ƕ����鉹��ID��ݒ�
					m_soundID = SoundID::NOT_MOVE;
				}
			}
		}
		break;
	}
	case State::BOX_MOVING:
	{
		//�{�b�N�X�̏�Ԃ��ҋ@��ԂȂ�
		if (m_movingBox->GetCurrentState() == Box::State::STAY)
		{
			//�G�t�F�N�g�̏������Ɛݒ�
			m_boxEffect->Initialize(
				0.75f, 
				m_movingBox->GetPosition()
			);
			m_boxEffect->SetRenderState(
				camera->GetEyePosition(),
				camera->GetViewMatrix(),
				camera->GetProjectionMatrix()
			);

			if (m_movingBox->GetType() == Box::Type::CLEAR_1)
			{
				//�N���A�����̃{�b�N�X�̈ʒu���擾����
				m_clearBox1Pos = m_movingBox->GetPosition();
			}
			else if (m_movingBox->GetType() == Box::Type::CLEAR_2)
			{
				//�N���A�����̃{�b�N�X�̈ʒu���擾����
				m_clearBox2Pos = m_movingBox->GetPosition();
			}

			//�󂭏ꏊ�����݂̈ʒu�ɐݒ肷��
			m_nullPos = m_movingBox->GetStartPos();
			m_movingBox = nullptr;
			m_state = State::FINISH;
		}
		break;
	}
	case State::FINISH:
		//���������̉���ݒ肷��
		m_soundID = SoundID::MOVE;

		//�N���A�����̃{�b�N�X�ƃN���A�ʒu���ꏏ��������
		if (m_clearBox1Pos == m_goalBox1Pos && m_clearBox2Pos == m_goalBox2Pos)
		{
			//�N���A�����̂ŃV�[����ς���
			m_changeScene = true;
		}
		
		//�{�b�N�X�̏�Ԃ�ҋ@��Ԃɂ���
		m_state = State::STAY;
		break;
	}

	//�{�b�N�X�̍X�V
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			box->Update(timer, scalingRatio);
		}
	}

	//�G�t�F�N�g�̍X�V
	m_boxEffect->Update(timer);
}

void BoxManager::Render(DebugCamera* camera)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//�E�B���h�E�T�C�Y�̎擾
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	//�ϊ��s��̐錾(���[���h�A�r���[�A�ˉe(�v���W�F�N�V����))
	Matrix world, view, projection;

	//���[���h�s��̏�����(�P�ʍs��)
	world = Matrix::Identity;
	view = camera->GetViewMatrix();						//�f�o�b�O�J�������g��

	//�v���W�F�N�V����
	constexpr float field0fView = XMConvertToRadians(45.0f);	//��p
	float aspectRatio = width / height;                         //��ʏc����
	float nearPlane = 1.0f;										//�J���������ԋ߂����e��
	float farPlane = 150.0f;									//�J���������ԉ������e��
	projection = Matrix::CreatePerspectiveFieldOfView
	(field0fView, aspectRatio, nearPlane, farPlane);

	//�{�b�N�X�̕`��
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			box->Render(camera);
		}
	}

	//TODO::PlayScene�Ɉڍs
	Matrix goalBox1Pos = Matrix::CreateTranslation(m_goalBox1Pos);
	Matrix goalBox2Pos = Matrix::CreateTranslation(m_goalBox2Pos);
	//�N���A�ʒu�������{�b�N�X�̕`��
	m_pGoalBox1->Draw(goalBox1Pos, view, projection, Color(1.0f, 1.0f, 0.0f, 0.5f), m_goalframe_1.Get());
	m_pGoalBox2->Draw(goalBox2Pos, view, projection, Color(0.0f, 1.0f, 0.0f, 0.25f), m_goalframe_2.Get());
	//�e�p�{�b�N�X�̕`��
	m_pShadowBox->Draw(Matrix::CreateTranslation(Vector3::Zero), view, projection, Color(0.5f, 0.5f, 0.5f, 0.5f));
	m_boxEffect->Render();
}

void BoxManager::Finalize()
{
	//�{�b�N�X�̍폜
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			box.reset();
			box->Finalize();
		}
	}

	//�{�b�N�X�̔z���j������
	m_pBox.clear();
}

//============================================================================
//�{�b�N�X�̈ꕔ���폜����
//============================================================================
void BoxManager::Delete(int element)
{
	m_pBox[element].reset();
}

//============================================================================
//�{�b�N�X�ׂ̗��󂢂Ă��邩���肷��
//============================================================================
bool BoxManager::VicinityNullPos(const DirectX::SimpleMath::Vector3& nullPos, const DirectX::SimpleMath::Vector3& boxPos)
{
	//�󂢂Ă���ꏊ��x���W�Ay���W�A�����W�������Ƃ�����i��
	if (nullPos.x == boxPos.x && nullPos.y == boxPos.y ||
		nullPos.y == boxPos.y && nullPos.z == boxPos.z ||
		nullPos.z == boxPos.z && nullPos.x == boxPos.x)
	{
			//�I�������{�b�N�X�ׂ̗�nullPos��������
		if (boxPos.x - 1.05f == nullPos.x ||
			boxPos.x + 1.05f == nullPos.x ||
			boxPos.y - 1.05f == nullPos.y ||
			boxPos.y + 1.05f == nullPos.y ||
			boxPos.z - 1.05f == nullPos.z ||
			boxPos.z + 1.05f == nullPos.z)
		{
			return true;
		}
	}
	return false;
}

//============================================================================
//�N���A�p�{�b�N�X�ׂ̗��S�[���ʒu�Ȃ̂����肷��
//============================================================================
bool BoxManager::VicinityClearPos(const DirectX::SimpleMath::Vector3& colorBoxPos, const DirectX::SimpleMath::Vector3& clearPos)
{
	//�N���A�����ƂȂ�{�b�N�X�ׂ̗��N���A�{�b�N�X��������
	if (clearPos.x == colorBoxPos.x && clearPos.y == colorBoxPos.y ||
		clearPos.y == colorBoxPos.y && clearPos.z == colorBoxPos.z ||
		clearPos.z == colorBoxPos.z && clearPos.x == colorBoxPos.x)
	{
		if (colorBoxPos.x - 1.05f == clearPos.x ||
			colorBoxPos.x + 1.05f == clearPos.x ||
			colorBoxPos.y - 1.05f == clearPos.y ||
			colorBoxPos.y + 1.05f == clearPos.y ||
			colorBoxPos.z - 1.05f == clearPos.z ||
			colorBoxPos.z + 1.05f == clearPos.z)
		{
			return true;
		}
	}
	return false;
}
