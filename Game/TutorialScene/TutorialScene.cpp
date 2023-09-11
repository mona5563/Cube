/*
チュートリアルシーン
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
コンストラクタ
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
デストラクタ
--------------------------------------------------*/
TutorialScene::~TutorialScene()
{
}

/*--------------------------------------------------
初期化
--------------------------------------------------*/
void TutorialScene::Initialize()
{
	//ADXのインスタンスを取得、初期化
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize("Resources/Audio/CubeBGM.acf",
		"Resources/Audio/CueSheet_0.acb"
	);

	//デバイスリソースのインスタンスを取得する
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	// D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	// コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	// スプライトバッチ::デバッグ情報の表示に必要
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(
		device,
		L"Resources/Fonts/SegoeUI_18.spritefont"
		);

	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//各画像の取得
	m_fadeImage = resourceManager.GetTexture(eTextureID::FADE); //フェード用
	m_explanationImage = resourceManager.GetTexture(eTextureID::EXPLANATION); //説明書
	m_nextImage = resourceManager.GetTexture(eTextureID::NEXT_SCENE_CLICK); //マウスクリック
}

/*--------------------------------------------------
更新
戻り値	:次のシーン番号
--------------------------------------------------*/
GAME_SCENE TutorialScene::Update(const DX::StepTimer& timer)
{
	// マウス入力情報を取得する
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	//経過時間を点滅時間にする
	float deltaTime = static_cast<float>(timer.GetElapsedSeconds());
	
	//描画時の点滅時間を計算する	
	if (m_isRender)
	{
		m_alphaTime -= deltaTime;
	}
	else
	{
		m_alphaTime += deltaTime;
	}
	
	//0.5秒で点滅表示するようにフラグを切り替える
	if (m_alphaTime > 0.5f)
	{
		m_isRender = true;
	}
	else if (m_alphaTime < 0.0f)
	{
		m_isRender = false;
	}
	
	//フェードアウトを行う
	if (mouseState.leftButton)
	{
		if (!m_fadeFlag)
		{
			//ボタンクリック時のSEを再生する
			m_pAdx2->Play(CRI_CUESHEET_0_KEYCLICK);
			//フェードフラグをtrueにする
			m_fadeFlag = true;
		}
	}

	//フェードアウトが完了したらシーンを移動する
	if (m_fadeAlpha >= 1.0f)
	{
		return GAME_SCENE::PLAY;
	}
	//フェードを行う
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
			//フェードの処理を行う
			m_fadeAlpha += 0.8f * deltaTimer;
		}

		//行き過ぎたら補正
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;
		}
	}

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
描画
--------------------------------------------------*/
void TutorialScene::Draw()
{
	//スプライトバッチによる描画を開始する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());
	
	//説明書の描画
	m_spriteBatch->Draw(
		m_explanationImage.Get(),
		m_backPosition
	);

	//点滅しているように見せる
	if (m_isRender)
	{
		//マウスクリックの描画
		m_spriteBatch->Draw(
			m_nextImage.Get(),
			m_backPosition
		);
	}
	
	//フェード時の色を指定する
	DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, m_fadeAlpha };
	//フェード用画像の描画
	m_spriteBatch->Draw(
		m_fadeImage.Get(),
		SimpleMath::Vector2::Zero,
		nullptr,
		color
	);
	//スプライトバッチによる描画を終了する
	m_spriteBatch->End();
}

/*--------------------------------------------------
終了処理
--------------------------------------------------*/
void TutorialScene::Finalize()
{
	//音を解放する
	m_pAdx2->Finalize();
}
