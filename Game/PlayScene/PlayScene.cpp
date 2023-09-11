/*
�v���C�V�[��
*/
#include "pch.h"

#include "DeviceResources.h"
#include "Game/ResourceManager.h"
#include "../GameMain.h"
#include "../../Libraries/MyLibraries/GridFloor.h"
#include "../../Libraries/MyLibraries/DebugCamera.h"

#include <WinUser.h> //GetWindowRect()���g�p���邽��
#include "Game.h" //�f�t�H���g�̃X�N���[���T�C�Y�����炤����

#include "GameDate/Collision.h"

//�����g�p�̂���
#include <limits>
#include <random>

#include <algorithm>
#include <iterator>
#include <vector>
#include <numeric>

#include "PlayScene.h"

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
PlayScene::PlayScene()
	:
	m_pAdx2(nullptr),
	m_bgmID(0),
	m_lightColor(DirectX::Colors::SkyBlue),
	m_colorTimer(0.0f),
	m_time(0.0f),
	m_seconds(0),
	m_minutes(0),
	m_hours(0),
	m_timePos(DirectX::SimpleMath::Vector2::Zero),
	m_fadeAlpha(0.0f),
	m_fadeFlag(false),
	m_moveCount(0),
	m_moveCountPos(DirectX::SimpleMath::Vector2::Zero),
	m_soundID(BoxManager::SoundID::NONE),
	m_scalingRatio(0.0f),
	m_pauseFlag(false)
{
	//�K�v�ȕϐ��̏���
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//�O���b�h�ƃf�o�b�O�J�����̏���
	m_pDebugCamera = std::make_unique<DebugCamera>();
	m_pGridFloor = std::make_unique<GridFloor>(device, context, 10, 10);

	//�R���C�_�[�̏���
	m_collider = std::make_unique<CapsuleCollider>();
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
PlayScene::~PlayScene()
{
	//�|�C���^�̉��
	if (m_pDebugCamera != nullptr)
	{
		m_pDebugCamera.reset();
		m_pDebugCamera = nullptr;
	}

	if (m_pGridFloor != nullptr)
	{
		m_pGridFloor.reset();
		m_pGridFloor = nullptr;
	}
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void PlayScene::Initialize()
{
	//���O��Ԃ̗��p
	using namespace DirectX::SimpleMath;

	//ADX�̃C���X�^���X���擾�A������
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize("Resources/Audio/CubeBGM.acf",
		"Resources/Audio/CueSheet_0.acb");

	//BGM�̍Đ�
	m_bgmID = m_pAdx2->Play(CRI_CUESHEET_0_BGM);

	//�K�v�ȕϐ��̏���
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
	// �X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");

	//�G�t�F�N�g�t�@�N�g��
	std::unique_ptr<DirectX::EffectFactory> factory = std::make_unique<DirectX::EffectFactory>(pDR->GetD3DDevice());

	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	factory->SetDirectory(L"Resources/Models");
	// �t�@�C�����w�肵�ă��f���f�[�^�ǂݍ���
	m_pBackModel = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/BackModel.cmo",
		*factory
	);
	factory.reset();

	//ResourceManager�̃C���X�^���X���擾
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();

	//�e�摜�̎擾
	m_fadeImage = resourceManager.GetTexture(eTextureID::FADE); //�t�F�[�h�p
	m_explanationImage = resourceManager.GetTexture(eTextureID::EXPLANATION); //������
	m_numTexture = resourceManager.GetTexture(eTextureID::NUMBER); //����
	m_moveCountTexture = resourceManager.GetTexture(eTextureID::MOVE_COUNT); //movecount����
	m_operationTexture = resourceManager.GetTexture(eTextureID::OPERATION); //����UI
	m_tabImage = resourceManager.GetTexture(eTextureID::TAB_OPEN); //Tab
	m_tabCloseImage = resourceManager.GetTexture(eTextureID::TAB_CLOSE); //Tab����

	//�`��ʒu���w��
	m_timePos = Vector2(1220, 10);
	m_moveCountPos = Vector2(110, 35);

	//�X�e�[�W�������_���ɂ���
	std::vector<int> boxV(27); //3*3*3�̃{�b�N�X����vector
	std::iota(boxV.begin(), boxV.end(), 0); // 0�`26 �܂ł̒l�𐶐�

	//�{�b�N�X��vector�̒��g���V���b�t������
	static std::random_device seedGenerator;
	static std::mt19937 randomGenerator{ seedGenerator() };
	std::shuffle(boxV.begin(), boxV.end(), randomGenerator);

	//�{�b�N�X�̐^��(�C���f�b�N�X��13)�̂��̂͏���
	std::vector<int>::iterator it = boxV.begin();
	while (it != boxV.end())
	{
		if (*it == 13)
		{
			it = boxV.erase(it);
		}
		else
		{
			++it;
		}
	}

	//�V���b�t�������{�b�N�Xvector�̐擪5���e�{�b�N�X�̃C���f�b�N�X�ɂ���
	int emptyBoxIndex = boxV[0]; //��ɂ���C���f�b�N�X
	int goalBoxIndex_1 = boxV[1]; //�S�[���{�b�N�X(���P)�̃C���f�b�N�X�p�ϐ�
	int goalBoxIndex_2 = boxV[2]; //�S�[���{�b�N�X(���Q)�̃C���f�b�N�X�p�ϐ�
	int clearBoxIndex_1 = boxV[3]; //�N���A�{�b�N�X(�P)�̃C���f�b�N�X�p�ϐ�
	int clearBoxIndex_2 = boxV[4]; //�N���A�{�b�N�X(�Q)�̃C���f�b�N�X�p�ϐ�

	//�{�b�N�X�̐����Ə�����
	m_pBox = std::make_unique<BoxManager>();
	m_pBox->Initialize(emptyBoxIndex, goalBoxIndex_1, goalBoxIndex_2, clearBoxIndex_1, clearBoxIndex_2);

}

/*--------------------------------------------------
�X�V
�߂�l	:���̃V�[���ԍ�
--------------------------------------------------*/
GAME_SCENE PlayScene::Update(const DX::StepTimer& timer)
{
	// �L�[���͏����擾����
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();
	keyStateTracker.Update(keyState);

	//[tab]�L�[��������Ă�����|�[�Y�t���O��؂�ւ���
	if (keyStateTracker.IsKeyPressed(DirectX::Keyboard::Keys::Tab))
	{
		if (m_pauseFlag)
		{
			m_pauseFlag = false;
		}
		else
		{
			m_pauseFlag = true;
		}
	}

	//�|�[�Y�t���O��false�Ȃ�Q�[�����X�V����
	if (!m_pauseFlag)
	{
		PlayUpdate(timer);
	}	

	//�t�F�[�h�A�E�g���s��
	if (m_pBox->GetChangeScene())
	{
		if (!m_fadeFlag)
		{
			m_fadeFlag = true;
		}
	}

	//�t�F�[�h�A�E�g������������V�[�����ړ�����
	if (m_fadeAlpha >= 1.0f)
	{
		return GAME_SCENE::RESULT;
	}

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
void PlayScene::Draw()
{
	using namespace DirectX::SimpleMath;

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//�E�B���h�E�T�C�Y�̎擾
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	//�ϊ��s��̐錾(���[���h�A�r���[�A�ˉe(�v���W�F�N�V����))
	DirectX::SimpleMath::Matrix world, view, projection;

	//���[���h�s��̏�����(�P�ʍs��)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pDebugCamera->GetViewMatrix();						//�f�o�b�O�J�������g��

	//�v���W�F�N�V����
	constexpr float field0fView = DirectX::XMConvertToRadians(45.0f);	//��p
	float aspectRatio = width / height;                         //��ʏc����
	float nearPlane = 1.0f;										//�J���������ԋ߂����e��
	float farPlane = 200.0f;									//�J���������ԉ������e��
	projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView
	(field0fView, aspectRatio, nearPlane, farPlane);

	m_projection = projection;
	
	//�O���b�h�̕\��
	//m_pGridFloor->Draw(context, m_commonState.get(), view, projection);

	//�V���̕`��
	m_pBackModel->UpdateEffects([&](DirectX::IEffect* effect)
		{
			DirectX::IEffectLights* lights = dynamic_cast<DirectX::IEffectLights*>(effect);
			if (lights)
			{
				// ���C�g�̉e�����Ȃ���
				lights->SetAmbientLightColor(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
				lights->SetLightEnabled(0, false);
				lights->SetLightEnabled(1, false);
				lights->SetLightEnabled(2, false);
			}
			DirectX::BasicEffect* basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (basicEffect)
			{
				// �G�~�b�V�����F��ݒ肷��
				basicEffect->SetEmissiveColor(m_lightColor);
			}
		});
	world *= Matrix::CreateRotationZ(0.707f);
	m_pBackModel->Draw(context, *m_commonState.get(), world, view, projection);

	//�{�b�N�X�̕`��
	world = DirectX::SimpleMath::Matrix::Identity;
	m_pBox->Render(m_pDebugCamera.get());

	//ResourceManager�̃C���X�^���X���擾
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//�����摜�̃T�C�Y���擾
	Vector2 numTextSize = resourceManager.GetTextureSize(eTextureID::NUMBER);
	//�����摜��؂蔲�����̕�
	long numRectWidth = (long)numTextSize.x / 11;

	//TODO::�v�Z��ʂ̊֐��ōs��
	//�o�ߎ��Ԃ̕`��
	const RECT numRect[11]
	{
		{ numRectWidth *  0, 0, numRectWidth *  1, (long)numTextSize.y},  //0
		{ numRectWidth *  1, 0, numRectWidth *  2, (long)numTextSize.y},  //1
		{ numRectWidth *  2, 0, numRectWidth *  3, (long)numTextSize.y},  //2
		{ numRectWidth *  3, 0, numRectWidth *  4, (long)numTextSize.y},  //3
		{ numRectWidth *  4, 0, numRectWidth *  5, (long)numTextSize.y},  //4
		{ numRectWidth *  5, 0, numRectWidth *  6, (long)numTextSize.y},  //5
		{ numRectWidth *  6, 0, numRectWidth *  7, (long)numTextSize.y},  //6
		{ numRectWidth *  7, 0, numRectWidth *  8, (long)numTextSize.y},  //7
		{ numRectWidth *  8, 0, numRectWidth *  9, (long)numTextSize.y},  //8
		{ numRectWidth *  9, 0, numRectWidth * 10, (long)numTextSize.y},  //9
		{ numRectWidth * 10, 0, numRectWidth * 11, (long)numTextSize.y},  //:
	};

	

	//�X�v���C�g�o�b�`�ɂ��`�恁����������������������������������������������������������������
	m_spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred, 
		m_commonState->NonPremultiplied()
	);

	//�o�ߎ��Ԃ̕`��
	DrawNum(m_timePos, numRect[m_seconds % 10]); //�b�F1�̈�
	DrawNum(Vector2(m_timePos.x - (50 * 1), m_timePos.y), numRect[m_seconds / 10] ); //�b�F10�̈�
	DrawNum(Vector2(m_timePos.x - (50 * 2), m_timePos.y), numRect[10]); //�u�F�v
	DrawNum(Vector2(m_timePos.x - (50 * 3), m_timePos.y), numRect[m_minutes % 10]); //���F1�̈�
	DrawNum(Vector2(m_timePos.x - (50 * 4), m_timePos.y), numRect[m_minutes / 10]);//���F10�̈�
	DrawNum(Vector2(m_timePos.x - (50 * 5), m_timePos.y), numRect[10]); //�u�F�v
	DrawNum(Vector2(m_timePos.x - (50 * 6), m_timePos.y), numRect[m_hours % 10]); //���F1�̈�
	DrawNum(Vector2(m_timePos.x - (50 * 7), m_timePos.y), numRect[m_hours / 10]); //���F10�̈�
	
	//�ړ��񐔂̕`��
	DrawNum(m_moveCountPos,numRect[(m_moveCount % 100) % 10]);//1�̈�
	DrawNum(Vector2(m_moveCountPos.x - (50 * 1), m_moveCountPos.y), numRect[(m_moveCount % 100) / 10]); //10�̈�
	DrawNum(Vector2(m_moveCountPos.x - (50 * 2), m_moveCountPos.y), numRect[(m_moveCount / 100)]); //100�̈�
	
	//moveCount�����ʒu
	DirectX::SimpleMath::Vector2 moveCountPos(-25, 0);
	//moveCount�����̕`��
	m_spriteBatch->Draw(
		m_moveCountTexture.Get(),
		moveCountPos,
		nullptr,
		DirectX::Colors::White,
		0.0f,
		Vector2::Zero,
		0.35f,
		DirectX::SpriteEffects_None
	);

	//����UI�̕`��
	m_spriteBatch->Draw(
		m_operationTexture.Get(),
		Vector2(10.0f, 500.0f),
		nullptr,
		DirectX::Colors::White,
		0.0f,
		Vector2::Zero,
		0.7f,
		DirectX::SpriteEffects_None
	);

	DirectX::SimpleMath::Vector2 tabPos(0.0f, 720.0f - 80.0f);
	if (m_pauseFlag)
	{
		//�|�[�Y���̕`�������
		//�w�i�𔼓����ȍ��F�ɂ���
		DirectX::XMVECTOR pauseBackColor = { 0.5f, 0.5f, 0.5f, 0.5f };
		//�t�F�[�h�p�摜��w�i�F�Ƃ��ė��p
		m_spriteBatch->Draw(
			m_fadeImage.Get(),
			Vector2::Zero,
			nullptr,
			pauseBackColor
		);

		//�E�B���h�E�̐^�񒆈ʒu
		DirectX::SimpleMath::Vector2 centerPos(width / 2.0f, height / 2.0f);
		//�������̕`��
		m_spriteBatch->Draw(
			m_explanationImage.Get(),
			centerPos,
			nullptr,
			DirectX::Colors::White,
			0.0f,
			centerPos,
			0.75f,
			DirectX::SpriteEffects_None
		);
		//�|�[�Y��ʏI������Ƃ���tab�L�[�����UI
		m_spriteBatch->Draw(
			m_tabCloseImage.Get(),
			tabPos,
			nullptr,
			DirectX::Colors::White
		);
	}
	else
	{
		//[tab]�L�[�̑���UI��`�悷��
		m_spriteBatch->Draw(
			m_tabImage.Get(),
			tabPos,
			nullptr,
			DirectX::Colors::White
		);
	}
	//�t�F�[�h�p�̐F�Ɠ����x���w��
	DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, m_fadeAlpha };
	//�t�F�[�h�p�摜�̕`��
	m_spriteBatch->Draw(
		m_fadeImage.Get(),
		Vector2::Zero,
		nullptr,
		color
	);
	//�`����I������
	m_spriteBatch->End();
}

/*--------------------------------------------------
�I������
--------------------------------------------------*/
void PlayScene::Finalize()
{
	//�����������
	m_pAdx2->Finalize();
	m_pBox->Finalize();
}

void PlayScene::PlayUpdate(const DX::StepTimer& timer)
{
	using namespace DirectX::SimpleMath;

	//�o�ߎ���
	float time = static_cast<float>(timer.GetElapsedSeconds());
	m_time += time;

	//�����蔻����s������������������������������������������������������������������
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//�J�����̍X�V
	m_pDebugCamera->Update();

	//	�E�B���h�E�T�C�Y�̎擾
	int width = pDR->GetOutputSize().right;
	int height = pDR->GetOutputSize().bottom;

	//	�}�E�X���͏����擾����
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();
	m_mouseStateTracker.Update(mouseState);

	//�t���X�N���[�����̊g�k��
	RECT rect;
	GetClientRect(pDR->GetHwnd(), &rect); //��������_�Ƃ�����ʂ̑��΍��W���擾
	//�g�k���̌v�Z
	m_scalingRatio = Game::SCREEN_H / static_cast<float>(rect.bottom);

	//�}�E�X�J�[�\���̃X�N���[�����W
	//�t���X�N���[�����ɑΉ����邽�߃X�N���[���g�k�̔{�����|����
	float mouseX = static_cast<float>(mouseState.x) * m_scalingRatio;
	float mouseY = static_cast<float>(mouseState.y) * m_scalingRatio;
	//�}�E�X�J�[�\���̈ʒu
	DirectX::SimpleMath::Vector3 nearMousePos = DirectX::SimpleMath::Vector3(mouseX, mouseY, 0.0f);
	DirectX::SimpleMath::Vector3 farMousePos = DirectX::SimpleMath::Vector3(mouseX, mouseY, 1.0f);
	//���[���h���W�ւ̕ϊ�
	DirectX::SimpleMath::Matrix view = m_pDebugCamera->GetViewMatrix();
	DirectX::SimpleMath::Matrix revMat = CreateMatrix_Screen2World(width, height, view, m_projection);
	//���C���̍쐬
	DirectX::SimpleMath::Vector3 rayStart = DirectX::SimpleMath::Vector3::Transform(nearMousePos, revMat);
	DirectX::SimpleMath::Vector3 rayEnd = DirectX::SimpleMath::Vector3::Transform(farMousePos, revMat);
	//���C�Ɍ����Ă��J�v�Z���̗p��
	m_mouseRay.bigin = rayStart;
	m_mouseRay.end = rayEnd;
	m_mouseRay.radius = 0.0f;

	//�{�b�N�X�̍X�V
	m_pBox->Update(
		timer,
		m_mouseStateTracker,
		m_mouseRay,
		m_pDebugCamera.get(),
		m_scalingRatio
	);

	//�o�ߎ��Ԃ̍X�V
	if (!(m_pBox->GetChangeScene()))
	{
		m_seconds = static_cast<int>(m_time);
		if (m_seconds >= 60)
		{
			m_time = 0.0f;
			m_minutes++;
		}
		if (m_minutes >= 60)
		{
			m_minutes = 0;
			m_hours++;
		}
		if (m_hours >= 60)
		{
			m_hours = 0;
		}
	}

	//�w�i�F��ς���
	if (m_pBox->GetVicinityClear())
	{
		m_colorTimer += time;
		if (m_colorTimer > 2.0f)
		{
			m_colorTimer = 2.0f;
		}
		m_lightColor = Color::Lerp(
			Color(0.529411793f, 0.807843208f, 0.921568692f),
			Color(1.000000000f, 1.000000000f, 0.878431439f),
			m_colorTimer / 2.0f
		);
	}

	//�ړ��񐔂��擾����
	m_moveCount = m_pBox->GetMovingCount();
	//�����擾����
	m_soundID = m_pBox->GetSoundID();
	//�T�E���hID�ɂ���ĉ���ς���
	if (m_soundID == BoxManager::SoundID::MOVE)
	{
		//��������
		m_pAdx2->Play(CRI_CUESHEET_0_ISMOVESE);
	}
	else if (m_soundID == BoxManager::SoundID::NOT_MOVE)
	{
		//�������Ȃ�������
		m_pAdx2->Play(CRI_CUESHEET_0_NOTMOVESE);
	}
}



//============================================================================
//���[���h���W�ւ̕ϊ��p�s��̐���
//============================================================================
DirectX::SimpleMath::Matrix PlayScene::CreateMatrix_Screen2World(
	int screen_w,
	int screen_h,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix projection
)
{
	//�r���[�|�[�g�X�P�[�����O�s����쐬
	DirectX::SimpleMath::Matrix viewport;
	viewport._11 = screen_w / 2.0f;
	viewport._22 = -screen_h / 2.0f;
	viewport._41 = screen_w / 2.0f;
	viewport._42 = screen_h / 2.0f;

	//�t�s����쐬
	DirectX::SimpleMath::Matrix invS = viewport.Invert();
	DirectX::SimpleMath::Matrix invP = projection.Invert();
	DirectX::SimpleMath::Matrix invV = view.Invert();

	//�w�r���[�|�[�g�X�P�[�����O�s��̋t�s��x�~�w�ˉe�s��̋t�s��x�~�w�r���[�s��̋t�s��x
	return invS * invP * invV;
}

//============================================================================
//������`�悷��
//============================================================================
void PlayScene::DrawNum(DirectX::SimpleMath::Vector2 pos, const RECT rect)
{
	using namespace DirectX::SimpleMath;

	m_spriteBatch->Draw(
		m_numTexture.Get(),
		pos,
		&rect,
		DirectX::Colors::White,
		0.0f,
		Vector2::Zero,
		0.5f,
		DirectX::SpriteEffects_None
	);
}