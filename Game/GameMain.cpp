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
// コンストラクタ
//-------------------------------------------------------------------
GameMain::GameMain()
	:m_nextScene(GAME_SCENE::TITLE)		// TODO::初期シーンの設定
	,m_pScene(nullptr)
	,m_seconds(0)
	,m_minutes(0)
	,m_hours(0)
	,m_moveNum(0)
{
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
GameMain::~GameMain()
{
	Finalize();
}

//-------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------
void GameMain::Initialize()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// キーボード関連
	m_keybord = std::make_unique<DirectX::Keyboard>();

	// マウス関連
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(pDR->GetHwnd());

	// シーン作成
	CreateScene();
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void GameMain::Update(const DX::StepTimer& timer)
{
	// キー入力情報を取得する
	DirectX::Keyboard::State keyState = m_keybord->GetState();

	// ESCキーで終了
	if (keyState.Escape)
	{
		PostQuitMessage(0);
	}

	// 次のシーンが設定されていたらシーン切り替え
	if (m_nextScene != GAME_SCENE::NONE)
	{
		// シーン削除
		DeleteScene();
		
		// シーン作成
		CreateScene();
	}

	// 実態があれば更新
	if (m_pScene != nullptr)
	{
		m_nextScene = m_pScene->Update(timer);
	}
}

//-------------------------------------------------------------------
// 描画処理
//-------------------------------------------------------------------
void GameMain::Render()
{
	// 実態があれば描画
	if (m_pScene != nullptr)
	{
		m_pScene->Draw();
	}
}

//-------------------------------------------------------------------
// 後始末
//-------------------------------------------------------------------
void GameMain::Finalize()
{
	DeleteScene();
}

/*--------------------------------------------------
シーンの作成
--------------------------------------------------*/
void GameMain::CreateScene()
{
	// シーンが作成されているときは処理しない
	if (m_pScene != nullptr)
	{
		return;
	}

	// 次シーンの作成
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
			//リザルトシーンにデータを渡す
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
			// 例外なので処理を中断
			return;
		}
	}

	// 作成したシーンを初期化
	m_pScene->Initialize();
}

/*--------------------------------------------------
シーンの削除
--------------------------------------------------*/
void GameMain::DeleteScene()
{
	// シーンが作成されていなければ処理しない
	if (m_pScene == nullptr)
	{
		return;
	}

	if (m_nextScene == GAME_SCENE::RESULT)
	{
		PlayScene* playScene = dynamic_cast<PlayScene*>(m_pScene.get());
		//リザルトシーンに渡すためのデータを取得する
		if (playScene != nullptr)
		{
			m_seconds = playScene->GetSeconds();
			m_minutes = playScene->GetMinutes();
			m_hours = playScene->GetHours();

			m_moveNum = playScene->GetMoveCount();
		}
	}

	// 現シーンの終了処理
	m_pScene->Finalize();

	// 現シーンの削除
	m_pScene.reset();
	m_pScene = nullptr;
}
