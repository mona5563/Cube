/*
���U���g�V�[��
*/

#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

#include "../GameMain.h"
#include "DeviceResources.h"
#include "Game/ResourceManager.h"

#include "ResultScene.h"

#include <limits>
#include <random>

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
ResultScene::ResultScene()
	:
	m_pAdx2(nullptr),
	m_bgmID(0),
	m_backPos(DirectX::SimpleMath::Vector2::Zero),
	m_clearPos(DirectX::SimpleMath::Vector2::Zero),
	m_nextSceneImgPos(DirectX::SimpleMath::Vector2::Zero), 
	m_clearTimePos(DirectX::SimpleMath::Vector2::Zero),
	m_clearTimeTexPos(DirectX::SimpleMath::Vector2::Zero),
	m_moveCountPos(DirectX::SimpleMath::Vector2::Zero),
	m_moveCountTexPos(DirectX::SimpleMath::Vector2::Zero),
	m_confettiPos(DirectX::SimpleMath::Vector2::Zero),
	m_fadeAlpha(0.0f),
	m_fadeFlag(false),
	m_seconds(0),
	m_minutes(0),
	m_hours(0),
	m_moveCount(0),
	m_clearImageSize(DirectX::SimpleMath::Vector2::Zero),
	m_nextImageSize(DirectX::SimpleMath::Vector2::Zero)
{
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
ResultScene::~ResultScene()
{
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void ResultScene::Initialize()
{
	using namespace DirectX;

	//ADX�̃C���X�^���X���擾�A������
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize(
		"Resources/Audio/CubeBGM.acf",
		"Resources/Audio/CueSheet_0.acb"
	);

	//BGM�̍Đ�
	m_bgmID = m_pAdx2->Play(CRI_CUESHEET_0_FANFARE);

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
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
	m_backImage = resourceManager.GetTexture(eTextureID::BACK); //�w�i
	m_clearImage = resourceManager.GetTexture(eTextureID::CLEAR); //clear����
	m_nextSceneImage = resourceManager.GetTexture(eTextureID::NEXT_SCENE_KEY); //left mouse click����
	m_numTexture = resourceManager.GetTexture(eTextureID::NUMBER); //����
	m_clearTimeTexture = resourceManager.GetTexture(eTextureID::CLEAR_TIME); //cleartime����
	m_moveCountTexture = resourceManager.GetTexture(eTextureID::MOVE_COUNT); //movecount����
	m_confetti = resourceManager.GetTexture(eTextureID::CONFETTI); //������

	//�e�N�X�`���T�C�Y�̎擾
	m_clearImageSize = resourceManager.GetTextureSize(eTextureID::CLEAR); //clear����
	m_nextImageSize = resourceManager.GetTextureSize(eTextureID::NEXT_SCENE_KEY); //left mouse click����

	//�e�e�N�X�`���̈ʒu
	m_clearPos = SimpleMath::Vector2(640, 360 - 150); //clear����
	m_backPos = SimpleMath::Vector2::Zero; //�w�i
	m_nextSceneImgPos = SimpleMath::Vector2(640, 550); //left mouse click
	m_clearTimePos = SimpleMath::Vector2(1100, 360 + 50); //�N���A�^�C��(�b�P�̈�)�F��ʒu
	m_clearTimeTexPos = SimpleMath::Vector2(75, 360 + 35); //cleartime����
	m_moveCountPos = SimpleMath::Vector2(850, 360 - 50); //�ړ���(1�̈�)�F�
	m_moveCountTexPos = SimpleMath::Vector2(75, 360 - 70); //movecount����
	m_confettiPos = SimpleMath::Vector2::Zero; //������
}

/*--------------------------------------------------
�X�V
�߂�l	:���̃V�[���ԍ�
--------------------------------------------------*/
GAME_SCENE ResultScene::Update(const DX::StepTimer& timer)
{
	using namespace DirectX;

	//�}�E�X���͏����擾����
	DirectX::Mouse::State mouseState = Mouse::Get().GetState();
	m_mouseStateTracker.Update(mouseState);

	//�o�ߎ��Ԃ�w�i�̓����X�s�[�h�ɂ���
	float speed = static_cast<float>(timer.GetElapsedSeconds());
	
	//����������ɓ�����
	m_confettiPos.y += 45.0f * speed;
	if (m_confettiPos.y >= -720.0f * -1)
	{
		//�����Ⴊ�s���߂��Ă����猳�ɖ߂�
		m_confettiPos.y = -720.0f;
	}

	if (m_mouseStateTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		//���V�[���ɍs���Ƃ�
		if (!m_fadeFlag)
		{
			//�{�^���N���b�NSE�𗬂�
			m_pAdx2->Play(CRI_CUESHEET_0_KEYCLICK);
			//�t�F�[�h�t���O��true�ɂ���
			m_fadeFlag = true;
		}
	}

	//�t�F�[�h�A�E�g������������V�[�����ړ�����
	if (m_fadeAlpha >= 1.0f)
	{
		return GAME_SCENE::TITLE;
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

	//�w�i�ʒu�����E�ɗh�炷���ߗ������擾���A���f������
	m_backPos = SimpleMath::Vector2(m_backPos.x + Swing(), m_backPos.y);

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
�`��
--------------------------------------------------*/
void ResultScene::Draw()
{
	using namespace DirectX;

	//�X�v���C�g�o�b�`�ŕ`����J�n����
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());
	//�����؂蔲��
	const RECT numRect[11]
	{
		{   0, 0,  100, 150},  //0
		{ 100, 0,  200, 150},  //1
		{ 200, 0,  300, 150},  //2
		{ 300, 0,  400, 150},  //3
		{ 400, 0,  500, 150},  //4
		{ 500, 0,  600, 150},  //5
		{ 600, 0,  700, 150},  //6
		{ 700, 0,  800, 150},  //7
		{ 800, 0,  900, 150},  //8
		{ 900, 0, 1000, 150},  //9
		{1000, 0, 1100, 150},  //:
	};

	//�w�i�̕`��
	m_spriteBatch->Draw(
		m_backImage.Get(),
		m_backPos
	);
	//�����I�ɕ`�悷�邽��
	if (m_backPos.x < 0.0f)
	{
		//x��0��菬����������A�E�̕��ֈʒu���ړ�������
		m_spriteBatch->Draw(
			m_backImage.Get(),
			DirectX::SimpleMath::Vector2(m_backPos.x + 1280.0f, m_backPos.y)
		);
	}
	else
	{
		//���ɗ����Ă���
		m_spriteBatch->Draw(
			m_backImage.Get(),
			DirectX::SimpleMath::Vector2(m_backPos.x - 1280.0f, m_backPos.y)
		);
	}

	//������̕`��
	m_spriteBatch->Draw(
		m_confetti.Get(),
		m_confettiPos
	);
	//�����I�ɕ`�悷�邽��
	if (m_confettiPos.y < 0.0f)
	{
		//y��0��菬����������A���̕��ֈʒu���ړ�������
		m_spriteBatch->Draw(
			m_confetti.Get(),
			DirectX::SimpleMath::Vector2(m_confettiPos.x, m_confettiPos.y + 720.0f)
		);
	}
	else
	{
		//y��0���傫��������A�X�N���[������Ɉʒu��߂�
		m_spriteBatch->Draw(
			m_confetti.Get(),
			DirectX::SimpleMath::Vector2(m_confettiPos.x, m_confettiPos.y - 720.0f)
		);
	}

	//�N���A�����̕`��
	m_spriteBatch->Draw(
		m_clearImage.Get(),
		m_clearPos,
		nullptr,
		Colors::White,
		0.0f,
		SimpleMath::Vector2(m_clearImageSize.x / 2.0f, m_clearImageSize.y / 2.0f),
		1.0f,
		SpriteEffects_None
	);
	//���V�[������̕`��
	m_spriteBatch->Draw(
		m_nextSceneImage.Get(),
		m_nextSceneImgPos, 
		nullptr,
		Colors::White,
		0.0f,
		SimpleMath::Vector2(m_nextImageSize.x / 2.0f, m_nextImageSize.y / 2.0f),
		2.0f,
		SpriteEffects_None
	);

	//�o�ߎ��Ԃ̕`��
	DrawNum(m_clearTimePos, numRect[m_seconds % 10]); //�b�F1�̈�
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 1), m_clearTimePos.y), numRect[m_seconds / 10]); //�b�F10�̈�
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 2), m_clearTimePos.y), numRect[10]); //�u�F�v	
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 3), m_clearTimePos.y), numRect[m_minutes % 10]); //���F1�̈�
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 4), m_clearTimePos.y),	numRect[m_minutes / 10]); //���F10�̈�
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 5), m_clearTimePos.y), numRect[10]); //�u�F�v
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 6), m_clearTimePos.y), numRect[m_hours % 10]); //���F1�̈�	
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 7), m_clearTimePos.y), numRect[m_hours / 10]); //���F10�̈�
	
	//clearTime�����̕`��
	m_spriteBatch->Draw(
		m_clearTimeTexture.Get(), 
		m_clearTimeTexPos
	);

	//�ړ��񐔂̕`��
	DrawNum(m_moveCountPos, numRect[(m_moveCount % 100) % 10]); //1�̈�
	DrawNum(SimpleMath::Vector2(m_moveCountPos.x - (50 * 1), m_moveCountPos.y), numRect[(m_moveCount % 100) / 10]); //10�̈�
	DrawNum(SimpleMath::Vector2(m_moveCountPos.x - (50 * 2), m_moveCountPos.y), numRect[(m_moveCount / 100)]); //100�̈�

	//moveCount�����̕`��
	m_spriteBatch->Draw(
		m_moveCountTexture.Get(),
		m_moveCountTexPos
	);

	//�t�F�[�h�̃A���t�@�l���w��
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
void ResultScene::Finalize()
{
}

/*--------------------------------------------------
�����̕`��
--------------------------------------------------*/
void ResultScene::DrawNum(DirectX::SimpleMath::Vector2 pos, const RECT rect)
{
	m_spriteBatch->Draw(
		m_numTexture.Get(),
		pos,
		&rect,
		DirectX::Colors::White,
		0.0f,
		DirectX::SimpleMath::Vector2::Zero,
		0.5f,
		DirectX::SpriteEffects_None
	);
}

/*--------------------------------------------------
���E�ɐU��ʂ̗������o��
--------------------------------------------------*/
float ResultScene::Swing()
{
	//�w�i�����E�ɗh�炷���߂̗����𐶐��E����������
	static std::random_device seedGenerator;
	static std::mt19937 randomGenerator{ seedGenerator()};
	static std::uniform_real_distribution<float> distribution{ -1.0f, 1.0f };

	//�U���ʂ𗐐��Ō��߂�
	float vector = distribution(randomGenerator);

	return vector;
}
