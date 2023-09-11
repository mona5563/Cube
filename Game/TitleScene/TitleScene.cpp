/*
�^�C�g���V�[��
*/

#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

#include "../GameMain.h"
#include "DeviceResources.h"

#include "TitleScene.h"

#include "Game/ResourceManager.h"

using namespace DirectX;

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
TitleScene::TitleScene()
	:
	m_pAdx2(nullptr),
	m_bgmID(0),
	m_backPosition(DirectX::SimpleMath::Vector2::Zero),
	m_fadeAlpha(0.0f),
	m_fadeFlag(false),
	m_logoImageSize(SimpleMath::Vector2::Zero),
	m_nextImageSize(SimpleMath::Vector2::Zero)
{
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
TitleScene::~TitleScene()
{
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void TitleScene::Initialize()
{
	//ADX�̃C���X�^���X���擾�A������
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize("Resources/Audio/CubeBGM.acf",
						"Resources/Audio/CueSheet_0.acb"
	);

	//BGM�̍Đ�
	m_bgmID = m_pAdx2->Play(CRI_CUESHEET_0_BGM);

	//�f�o�C�X���\�[�X�̃C���X�^���X���擾����
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	//�X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");

	//ResourceManager�̃C���X�^���X���擾
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();

	//�e�摜�̎擾
	m_nextSceneImage = resourceManager.GetTexture(eTextureID::NEXT_SCENE_KEY); //left mouse click
	m_fadeImage = resourceManager.GetTexture(eTextureID::FADE); //�t�F�[�h
	m_titleBack = resourceManager.GetTexture(eTextureID::BACK); //�^�C�g���w�i
	m_titleLogo = resourceManager.GetTexture(eTextureID::TITLE_LOGO); //�^�C�g�����S

	//�e�N�X�`���T�C�Y�̎擾
	m_logoImageSize = resourceManager.GetTextureSize(eTextureID::TITLE_LOGO); //�^�C�g�����S
	m_nextImageSize = resourceManager.GetTextureSize(eTextureID::NEXT_SCENE_KEY); //left mouse click
}

/*--------------------------------------------------
�X�V
�߂�l	:���̃V�[���ԍ�
--------------------------------------------------*/
GAME_SCENE TitleScene::Update(const DX::StepTimer& timer)
{
	//�}�E�X���͏����擾����
	DirectX::Mouse::State mouseState = Mouse::Get().GetState();
	m_mouseStateTracker.Update(mouseState);

	//�o�ߎ��Ԃ�w�i�̓����X�s�[�h�ɂ���
	float speed = static_cast<float>(timer.GetElapsedSeconds());

	//�w�i�����ɓ�����
	m_backPosition.x -= 45.0f * speed;
	if (m_backPosition.x <= 1280.0f * -1)
	{
		//�w�i���s���߂��Ă����猳�ɖ߂�
		m_backPosition.x = 1280.0f;
	}

	//�t�F�[�h�A�E�g
	if(m_mouseStateTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
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
		return GAME_SCENE::TUTORIAL;
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
void TitleScene::Draw()
{
	//�X�v���C�g�o�b�`�ɂ��`����J�n����
	m_spriteBatch->Begin(
		SpriteSortMode_Deferred,
		m_commonState->NonPremultiplied()
	);

	SimpleMath::Vector2 logoPos(640, 360);
	SimpleMath::Vector2 nextImagePos(650, 500);

	//�w�i�̕`��
	m_spriteBatch->Draw(m_titleBack.Get(), m_backPosition);
	if (m_backPosition.x < 0.0f)
	{
		//x���W��0��菬�����Ȃ������ʉE���ɉ摜���ړ�������
		m_spriteBatch->Draw(
			m_titleBack.Get(),
			DirectX::SimpleMath::Vector2(m_backPosition.x + 1280.0f, m_backPosition.y)
		);
	}
	else
	{
		//���ɗ����Ă���
		m_spriteBatch->Draw(
			m_titleBack.Get(),
			DirectX::SimpleMath::Vector2(m_backPosition.x - 1280.0f, m_backPosition.y)
		);
	}

	//�^�C�g�����S�̕`��
	m_spriteBatch->Draw(
		m_titleLogo.Get(),
		logoPos,
		nullptr,
		Colors::White,
		0.0f,
		SimpleMath::Vector2(m_logoImageSize.x / 2.0f, m_logoImageSize.y / 2.0f),
		1.0f,
		SpriteEffects_None
	);

	//���V�[���ֈڍs�̂��߂̎w���摜�̕`��
	m_spriteBatch->Draw(
		m_nextSceneImage.Get(),
		nextImagePos,
		nullptr,
		Colors::White,
		0.0f,
		SimpleMath::Vector2(m_nextImageSize.x / 2.0f, m_nextImageSize.y / 2.0f),
		2.0f,
		SpriteEffects_None
	);

	//�t�F�[�h���̐F���w��
	DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, m_fadeAlpha};
	//�t�F�[�h�p�摜�̕`��
	m_spriteBatch->Draw(m_fadeImage.Get(), SimpleMath::Vector2::Zero, nullptr, color);

	//�X�v���C�g�o�b�`�ɂ��`����I������
	m_spriteBatch->End();
}

/*--------------------------------------------------
�I������
--------------------------------------------------*/
void TitleScene::Finalize()
{
	//�����������
	m_pAdx2->Finalize();
}
