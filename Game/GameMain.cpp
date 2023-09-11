//
// Scene.cpp
//
#include "pch.h"

#include "DeviceResources.h"
#include "GameMain.h"

#include "Libraries/MyLibraries/DebugCamera.h"
#include "Libraries/MyLibraries/GridFloor.h"

#include "Game/TitleScene/TitleScene.h"
#include "Game//TutorialScene/TutorialScene.h"
#include "Game/PlayScene/PlayScene.h"
#include "Game/ResultScene/ResultScene.h"

//-------------------------------------------------------------------
// �R���X�g���N�^
//-------------------------------------------------------------------
GameMain::GameMain()
	:m_nextScene(GAME_SCENE::TITLE)		// TODO::�����V�[���̐ݒ�
	,m_pScene(nullptr)
	,m_seconds(0)
	,m_minutes(0)
	,m_hours(0)
	,m_moveNum(0)
{
}

//-------------------------------------------------------------------
// �f�X�g���N�^
//-------------------------------------------------------------------
GameMain::~GameMain()
{
	Finalize();
}

//-------------------------------------------------------------------
// ����������
//-------------------------------------------------------------------
void GameMain::Initialize()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// �L�[�{�[�h�֘A
	m_keybord = std::make_unique<DirectX::Keyboard>();

	// �}�E�X�֘A
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(pDR->GetHwnd());

	// �V�[���쐬
	CreateScene();
}

//-------------------------------------------------------------------
// �X�V����
//-------------------------------------------------------------------
void GameMain::Update(const DX::StepTimer& timer)
{
	// �L�[���͏����擾����
	DirectX::Keyboard::State keyState = m_keybord->GetState();

	// ESC�L�[�ŏI��
	if (keyState.Escape)
	{
		PostQuitMessage(0);
	}

	// ���̃V�[�����ݒ肳��Ă�����V�[���؂�ւ�
	if (m_nextScene != GAME_SCENE::NONE)
	{
		// �V�[���폜
		DeleteScene();
		
		// �V�[���쐬
		CreateScene();
	}

	// ���Ԃ�����΍X�V
	if (m_pScene != nullptr)
	{
		m_nextScene = m_pScene->Update(timer);
	}
}

//-------------------------------------------------------------------
// �`�揈��
//-------------------------------------------------------------------
void GameMain::Render()
{
	// ���Ԃ�����Ε`��
	if (m_pScene != nullptr)
	{
		m_pScene->Draw();
	}
}

//-------------------------------------------------------------------
// ��n��
//-------------------------------------------------------------------
void GameMain::Finalize()
{
	DeleteScene();
}

/*--------------------------------------------------
�V�[���̍쐬
--------------------------------------------------*/
void GameMain::CreateScene()
{
	// �V�[�����쐬����Ă���Ƃ��͏������Ȃ�
	if (m_pScene != nullptr)
	{
		return;
	}

	// ���V�[���̍쐬
	switch (m_nextScene)
	{
		case GAME_SCENE::TITLE:
		{
			m_pScene = std::make_unique<TitleScene>();
			break;
		}
		case GAME_SCENE::TUTORIAL:
		{
			m_pScene = std::make_unique <TutorialScene>();
			break;
		}
		case GAME_SCENE::PLAY:
		{
			m_pScene = std::make_unique<PlayScene>();
			break;
		}
		case GAME_SCENE::RESULT:
		{
			m_pScene = std::make_unique<ResultScene>();

			ResultScene* resultScene = dynamic_cast<ResultScene*>(m_pScene.get());
			//���U���g�V�[���Ƀf�[�^��n��
			if (resultScene != nullptr)
			{
				resultScene->SetSecond(m_seconds);
				resultScene->SetMinute(m_minutes);
				resultScene->SetHour(m_hours);

				resultScene->SetMoveCount(m_moveNum);
			}

			break;
		}
		default:
		{
			// ��O�Ȃ̂ŏ����𒆒f
			return;
		}
	}

	// �쐬�����V�[����������
	m_pScene->Initialize();
}

/*--------------------------------------------------
�V�[���̍폜
--------------------------------------------------*/
void GameMain::DeleteScene()
{
	// �V�[�����쐬����Ă��Ȃ���Ώ������Ȃ�
	if (m_pScene == nullptr)
	{
		return;
	}

	if (m_nextScene == GAME_SCENE::RESULT)
	{
		PlayScene* playScene = dynamic_cast<PlayScene*>(m_pScene.get());
		//���U���g�V�[���ɓn�����߂̃f�[�^���擾����
		if (playScene != nullptr)
		{
			m_seconds = playScene->GetSeconds();
			m_minutes = playScene->GetMinutes();
			m_hours = playScene->GetHours();

			m_moveNum = playScene->GetMoveCount();
		}
	}

	// ���V�[���̏I������
	m_pScene->Finalize();

	// ���V�[���̍폜
	m_pScene.reset();
	m_pScene = nullptr;
}
