/*
タイトルシーン
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
コンストラクタ
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
デストラクタ
--------------------------------------------------*/
TitleScene::~TitleScene()
{
}

/*--------------------------------------------------
初期化
--------------------------------------------------*/
void TitleScene::Initialize()
{
	//ADXのインスタンスを取得、初期化
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize("Resources/Audio/CubeBGM.acf",
						"Resources/Audio/CueSheet_0.acb"
	);

	//BGMの再生
	m_bgmID = m_pAdx2->Play(CRI_CUESHEET_0_BGM);

	//デバイスリソースのインスタンスを取得する
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	//スプライトバッチ::デバッグ情報の表示に必要
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");

	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();

	//各画像の取得
	m_nextSceneImage = resourceManager.GetTexture(eTextureID::NEXT_SCENE_KEY); //left mouse click
	m_fadeImage = resourceManager.GetTexture(eTextureID::FADE); //フェード
	m_titleBack = resourceManager.GetTexture(eTextureID::BACK); //タイトル背景
	m_titleLogo = resourceManager.GetTexture(eTextureID::TITLE_LOGO); //タイトルロゴ

	//テクスチャサイズの取得
	m_logoImageSize = resourceManager.GetTextureSize(eTextureID::TITLE_LOGO); //タイトルロゴ
	m_nextImageSize = resourceManager.GetTextureSize(eTextureID::NEXT_SCENE_KEY); //left mouse click
}

/*--------------------------------------------------
更新
戻り値	:次のシーン番号
--------------------------------------------------*/
GAME_SCENE TitleScene::Update(const DX::StepTimer& timer)
{
	//マウス入力情報を取得する
	DirectX::Mouse::State mouseState = Mouse::Get().GetState();
	m_mouseStateTracker.Update(mouseState);

	//経過時間を背景の動くスピードにする
	float speed = static_cast<float>(timer.GetElapsedSeconds());

	//背景を左に動かす
	m_backPosition.x -= 45.0f * speed;
	if (m_backPosition.x <= 1280.0f * -1)
	{
		//背景が行き過ぎていたら元に戻す
		m_backPosition.x = 1280.0f;
	}

	//フェードアウト
	if(m_mouseStateTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
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
		return GAME_SCENE::TUTORIAL;
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
void TitleScene::Draw()
{
	//スプライトバッチによる描画を開始する
	m_spriteBatch->Begin(
		SpriteSortMode_Deferred,
		m_commonState->NonPremultiplied()
	);

	SimpleMath::Vector2 logoPos(640, 360);
	SimpleMath::Vector2 nextImagePos(650, 500);

	//背景の描画
	m_spriteBatch->Draw(m_titleBack.Get(), m_backPosition);
	if (m_backPosition.x < 0.0f)
	{
		//x座標が0より小さくなったら画面右側に画像を移動させる
		m_spriteBatch->Draw(
			m_titleBack.Get(),
			DirectX::SimpleMath::Vector2(m_backPosition.x + 1280.0f, m_backPosition.y)
		);
	}
	else
	{
		//左に流していく
		m_spriteBatch->Draw(
			m_titleBack.Get(),
			DirectX::SimpleMath::Vector2(m_backPosition.x - 1280.0f, m_backPosition.y)
		);
	}

	//タイトルロゴの描画
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

	//次シーンへ移行のための指示画像の描画
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

	//フェード時の色を指定
	DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, m_fadeAlpha};
	//フェード用画像の描画
	m_spriteBatch->Draw(m_fadeImage.Get(), SimpleMath::Vector2::Zero, nullptr, color);

	//スプライトバッチによる描画を終了する
	m_spriteBatch->End();
}

/*--------------------------------------------------
終了処理
--------------------------------------------------*/
void TitleScene::Finalize()
{
	//音を解放する
	m_pAdx2->Finalize();
}
