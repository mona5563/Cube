/*
リザルトシーン
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
コンストラクタ
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
デストラクタ
--------------------------------------------------*/
ResultScene::~ResultScene()
{
}

/*--------------------------------------------------
初期化
--------------------------------------------------*/
void ResultScene::Initialize()
{
	using namespace DirectX;

	//ADXのインスタンスを取得、初期化
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize(
		"Resources/Audio/CubeBGM.acf",
		"Resources/Audio/CueSheet_0.acb"
	);

	//BGMの再生
	m_bgmID = m_pAdx2->Play(CRI_CUESHEET_0_FANFARE);

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//コモンステート::D3Dレンダリング状態オブジェクト
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
	m_backImage = resourceManager.GetTexture(eTextureID::BACK); //背景
	m_clearImage = resourceManager.GetTexture(eTextureID::CLEAR); //clear文字
	m_nextSceneImage = resourceManager.GetTexture(eTextureID::NEXT_SCENE_KEY); //left mouse click文字
	m_numTexture = resourceManager.GetTexture(eTextureID::NUMBER); //数字
	m_clearTimeTexture = resourceManager.GetTexture(eTextureID::CLEAR_TIME); //cleartime文字
	m_moveCountTexture = resourceManager.GetTexture(eTextureID::MOVE_COUNT); //movecount文字
	m_confetti = resourceManager.GetTexture(eTextureID::CONFETTI); //紙吹雪

	//テクスチャサイズの取得
	m_clearImageSize = resourceManager.GetTextureSize(eTextureID::CLEAR); //clear文字
	m_nextImageSize = resourceManager.GetTextureSize(eTextureID::NEXT_SCENE_KEY); //left mouse click文字

	//各テクスチャの位置
	m_clearPos = SimpleMath::Vector2(640, 360 - 150); //clear文字
	m_backPos = SimpleMath::Vector2::Zero; //背景
	m_nextSceneImgPos = SimpleMath::Vector2(640, 550); //left mouse click
	m_clearTimePos = SimpleMath::Vector2(1100, 360 + 50); //クリアタイム(秒１の位)：基準位置
	m_clearTimeTexPos = SimpleMath::Vector2(75, 360 + 35); //cleartime文字
	m_moveCountPos = SimpleMath::Vector2(850, 360 - 50); //移動回数(1の位)：基準
	m_moveCountTexPos = SimpleMath::Vector2(75, 360 - 70); //movecount文字
	m_confettiPos = SimpleMath::Vector2::Zero; //紙吹雪
}

/*--------------------------------------------------
更新
戻り値	:次のシーン番号
--------------------------------------------------*/
GAME_SCENE ResultScene::Update(const DX::StepTimer& timer)
{
	using namespace DirectX;

	//マウス入力情報を取得する
	DirectX::Mouse::State mouseState = Mouse::Get().GetState();
	m_mouseStateTracker.Update(mouseState);

	//経過時間を背景の動くスピードにする
	float speed = static_cast<float>(timer.GetElapsedSeconds());
	
	//紙吹雪を下に動かす
	m_confettiPos.y += 45.0f * speed;
	if (m_confettiPos.y >= -720.0f * -1)
	{
		//紙吹雪が行き過ぎていたら元に戻す
		m_confettiPos.y = -720.0f;
	}

	if (m_mouseStateTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
	{
		//次シーンに行くとき
		if (!m_fadeFlag)
		{
			//ボタンクリックSEを流す
			m_pAdx2->Play(CRI_CUESHEET_0_KEYCLICK);
			//フェードフラグをtrueにする
			m_fadeFlag = true;
		}
	}

	//フェードアウトが完了したらシーンを移動する
	if (m_fadeAlpha >= 1.0f)
	{
		return GAME_SCENE::TITLE;
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

	//背景位置を左右に揺らすため乱数を取得し、反映させる
	m_backPos = SimpleMath::Vector2(m_backPos.x + Swing(), m_backPos.y);

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
描画
--------------------------------------------------*/
void ResultScene::Draw()
{
	using namespace DirectX;

	//スプライトバッチで描画を開始する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());
	//数字切り抜き
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

	//背景の描画
	m_spriteBatch->Draw(
		m_backImage.Get(),
		m_backPos
	);
	//持続的に描画するため
	if (m_backPos.x < 0.0f)
	{
		//xが0より小さかったら、右の方へ位置を移動させる
		m_spriteBatch->Draw(
			m_backImage.Get(),
			DirectX::SimpleMath::Vector2(m_backPos.x + 1280.0f, m_backPos.y)
		);
	}
	else
	{
		//左に流していく
		m_spriteBatch->Draw(
			m_backImage.Get(),
			DirectX::SimpleMath::Vector2(m_backPos.x - 1280.0f, m_backPos.y)
		);
	}

	//紙吹雪の描画
	m_spriteBatch->Draw(
		m_confetti.Get(),
		m_confettiPos
	);
	//持続的に描画するため
	if (m_confettiPos.y < 0.0f)
	{
		//yが0より小さかったら、下の方へ位置を移動させる
		m_spriteBatch->Draw(
			m_confetti.Get(),
			DirectX::SimpleMath::Vector2(m_confettiPos.x, m_confettiPos.y + 720.0f)
		);
	}
	else
	{
		//yが0より大きかったら、スクリーンより上に位置を戻す
		m_spriteBatch->Draw(
			m_confetti.Get(),
			DirectX::SimpleMath::Vector2(m_confettiPos.x, m_confettiPos.y - 720.0f)
		);
	}

	//クリア文字の描画
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
	//次シーン操作の描画
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

	//経過時間の描画
	DrawNum(m_clearTimePos, numRect[m_seconds % 10]); //秒：1の位
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 1), m_clearTimePos.y), numRect[m_seconds / 10]); //秒：10の位
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 2), m_clearTimePos.y), numRect[10]); //「：」	
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 3), m_clearTimePos.y), numRect[m_minutes % 10]); //分：1の位
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 4), m_clearTimePos.y),	numRect[m_minutes / 10]); //分：10の位
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 5), m_clearTimePos.y), numRect[10]); //「：」
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 6), m_clearTimePos.y), numRect[m_hours % 10]); //時：1の位	
	DrawNum(SimpleMath::Vector2(m_clearTimePos.x - (50 * 7), m_clearTimePos.y), numRect[m_hours / 10]); //時：10の位
	
	//clearTime文字の描画
	m_spriteBatch->Draw(
		m_clearTimeTexture.Get(), 
		m_clearTimeTexPos
	);

	//移動回数の描画
	DrawNum(m_moveCountPos, numRect[(m_moveCount % 100) % 10]); //1の位
	DrawNum(SimpleMath::Vector2(m_moveCountPos.x - (50 * 1), m_moveCountPos.y), numRect[(m_moveCount % 100) / 10]); //10の位
	DrawNum(SimpleMath::Vector2(m_moveCountPos.x - (50 * 2), m_moveCountPos.y), numRect[(m_moveCount / 100)]); //100の位

	//moveCount文字の描画
	m_spriteBatch->Draw(
		m_moveCountTexture.Get(),
		m_moveCountTexPos
	);

	//フェードのアルファ値を指定
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
void ResultScene::Finalize()
{
}

/*--------------------------------------------------
数字の描画
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
左右に振る量の乱数を出す
--------------------------------------------------*/
float ResultScene::Swing()
{
	//背景を左右に揺らすための乱数を生成・初期化する
	static std::random_device seedGenerator;
	static std::mt19937 randomGenerator{ seedGenerator()};
	static std::uniform_real_distribution<float> distribution{ -1.0f, 1.0f };

	//振れる量を乱数で決める
	float vector = distribution(randomGenerator);

	return vector;
}
