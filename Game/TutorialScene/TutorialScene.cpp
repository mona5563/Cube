/*
�`���[�g���A���V�[��
*/

#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

#include "../GameMain.h"
#include "DeviceResources.h"
#include "Game/ResourceManager.h"

#include "TutorialScene.h"

using namespace DirectX;

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
TutorialScene::TutorialScene()
	:
	m_pAdx2(nullptr),
	m_backPosition(DirectX::SimpleMath::Vector2::Zero),
	m_alphaTime(0.0f),
	m_isRender(false),
	m_fadeAlpha(0.0f),
	m_fadeFlag(false)
{
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
TutorialScene::~TutorialScene()
{
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void TutorialScene::Initialize()
{
	//ADX�̃C���X�^���X���擾�A������
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize("Resources/Audio/CubeBGM.acf",
		"Resources/Audio/CueSheet_0.acb"
	);

	//�f�o�C�X���\�[�X�̃C���X�^���X���擾����
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	// �R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	// �X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(
		device,
		L"Resources/Fonts/SegoeUI_18.spritefont"
		);

	//ResourceManager�̃C���X�^���X���擾
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//�e�摜�̎擾
	m_fadeImage = resourceManager.GetTexture(eTextureID::FADE); //�t�F�[�h�p
	m_explanationImage = resourceManager.GetTexture(eTextureID::EXPLANATION); //������
	m_nextImage = resourceManager.GetTexture(eTextureID::NEXT_SCENE_CLICK); //�}�E�X�N���b�N
}

/*--------------------------------------------------
�X�V
�߂�l	:���̃V�[���ԍ�
--------------------------------------------------*/
GAME_SCENE TutorialScene::Update(const DX::StepTimer& timer)
{
	// �}�E�X���͏����擾����
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	//�o�ߎ��Ԃ�_�Ŏ��Ԃɂ���
	float deltaTime = static_cast<float>(timer.GetElapsedSeconds());
	
	//�`�掞�̓_�Ŏ��Ԃ��v�Z����	
	if (m_isRender)
	{
		m_alphaTime -= deltaTime;
	}
	else
	{
		m_alphaTime += deltaTime;
	}
	
	//0.5�b�œ_�ŕ\������悤�Ƀt���O��؂�ւ���
	if (m_alphaTime > 0.5f)
	{
		m_isRender = true;
	}
	else if (m_alphaTime < 0.0f)
	{
		m_isRender = false;
	}
	
	//�t�F�[�h�A�E�g���s��
	if (mouseState.leftButton)
	{
		if (!m_fadeFlag)
		{
			//�{�^���N���b�N����SE���Đ�����
			m_pAdx2->Play(CRI_CUESHEET_0_KEYCLICK);
			//�t�F�[�h�t���O��true�ɂ���
			m_fadeFlag = true;
		}
	}

	//�t�F�[�h�A�E�g������������V�[�����ړ�����
	if (m_fadeAlpha >= 1.0f)
	{
		return GAME_SCENE::PLAY;
	}
	//�t�F�[�h���s��
	if (m_fadeFlag)
	{
		float fps = (float)timer.GetFramesPerSecond();

		if (fps == 0.0f)
		{
			fps = 1.0f;
		}

		if (fps > 30.0f)
		{
			float deltaTimer = 1.0f / fps;
			//�t�F�[�h�̏������s��
			m_fadeAlpha += 0.8f * deltaTimer;
		}

		//�s���߂�����␳
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;
		}
	}

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
�`��
--------------------------------------------------*/
void TutorialScene::Draw()
{
	//�X�v���C�g�o�b�`�ɂ��`����J�n����
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());
	
	//�������̕`��
	m_spriteBatch->Draw(
		m_explanationImage.Get(),
		m_backPosition
	);

	//�_�ł��Ă���悤�Ɍ�����
	if (m_isRender)
	{
		//�}�E�X�N���b�N�̕`��
		m_spriteBatch->Draw(
			m_nextImage.Get(),
			m_backPosition
		);
	}
	
	//�t�F�[�h���̐F���w�肷��
	DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, m_fadeAlpha };
	//�t�F�[�h�p�摜�̕`��
	m_spriteBatch->Draw(
		m_fadeImage.Get(),
		SimpleMath::Vector2::Zero,
		nullptr,
		color
	);
	//�X�v���C�g�o�b�`�ɂ��`����I������
	m_spriteBatch->End();
}

/*--------------------------------------------------
�I������
--------------------------------------------------*/
void TutorialScene::Finalize()
{
	//�����������
	m_pAdx2->Finalize();
}
