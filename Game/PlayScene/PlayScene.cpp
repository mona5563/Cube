/*
プレイシーン
*/
#include "pch.h"

#include "DeviceResources.h"
#include "Game/ResourceManager.h"
#include "../GameMain.h"
#include "../../Libraries/MyLibraries/GridFloor.h"
#include "../../Libraries/MyLibraries/DebugCamera.h"

#include <WinUser.h> //GetWindowRect()を使用するため
#include "Game.h" //デフォルトのスクリーンサイズをもらうため

#include "GameDate/Collision.h"

//乱数使用のため
#include <limits>
#include <random>

#include <algorithm>
#include <iterator>
#include <vector>
#include <numeric>

#include "PlayScene.h"

/*--------------------------------------------------
コンストラクタ
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
	//必要な変数の準備
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//グリッドとデバッグカメラの準備
	m_pDebugCamera = std::make_unique<DebugCamera>();
	m_pGridFloor = std::make_unique<GridFloor>(device, context, 10, 10);

	//コライダーの準備
	m_collider = std::make_unique<CapsuleCollider>();
}

/*--------------------------------------------------
デストラクタ
--------------------------------------------------*/
PlayScene::~PlayScene()
{
	//ポインタの解放
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
初期化
--------------------------------------------------*/
void PlayScene::Initialize()
{
	//名前空間の利用
	using namespace DirectX::SimpleMath;

	//ADXのインスタンスを取得、初期化
	m_pAdx2 = &ADX2::GetInstance();
	m_pAdx2->Initialize("Resources/Audio/CubeBGM.acf",
		"Resources/Audio/CueSheet_0.acb");

	//BGMの再生
	m_bgmID = m_pAdx2->Play(CRI_CUESHEET_0_BGM);

	//必要な変数の準備
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
	// スプライトバッチ::デバッグ情報の表示に必要
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");

	//エフェクトファクトリ
	std::unique_ptr<DirectX::EffectFactory> factory = std::make_unique<DirectX::EffectFactory>(pDR->GetD3DDevice());

	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/Models");
	// ファイルを指定してモデルデータ読み込み
	m_pBackModel = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/BackModel.cmo",
		*factory
	);
	factory.reset();

	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();

	//各画像の取得
	m_fadeImage = resourceManager.GetTexture(eTextureID::FADE); //フェード用
	m_explanationImage = resourceManager.GetTexture(eTextureID::EXPLANATION); //説明書
	m_numTexture = resourceManager.GetTexture(eTextureID::NUMBER); //数字
	m_moveCountTexture = resourceManager.GetTexture(eTextureID::MOVE_COUNT); //movecount文字
	m_operationTexture = resourceManager.GetTexture(eTextureID::OPERATION); //操作UI
	m_tabImage = resourceManager.GetTexture(eTextureID::TAB_OPEN); //Tab
	m_tabCloseImage = resourceManager.GetTexture(eTextureID::TAB_CLOSE); //Tab閉じる

	//描画位置を指定
	m_timePos = Vector2(1220, 10);
	m_moveCountPos = Vector2(110, 35);

	//ステージをランダムにする
	std::vector<int> boxV(27); //3*3*3のボックス分のvector
	std::iota(boxV.begin(), boxV.end(), 0); // 0～26 までの値を生成

	//ボックスのvectorの中身をシャッフルする
	static std::random_device seedGenerator;
	static std::mt19937 randomGenerator{ seedGenerator() };
	std::shuffle(boxV.begin(), boxV.end(), randomGenerator);

	//ボックスの真ん中(インデックスが13)のものは消す
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

	//シャッフルしたボックスvectorの先頭5つを各ボックスのインデックスにする
	int emptyBoxIndex = boxV[0]; //空にするインデックス
	int goalBoxIndex_1 = boxV[1]; //ゴールボックス(旗１)のインデックス用変数
	int goalBoxIndex_2 = boxV[2]; //ゴールボックス(旗２)のインデックス用変数
	int clearBoxIndex_1 = boxV[3]; //クリアボックス(１)のインデックス用変数
	int clearBoxIndex_2 = boxV[4]; //クリアボックス(２)のインデックス用変数

	//ボックスの生成と初期化
	m_pBox = std::make_unique<BoxManager>();
	m_pBox->Initialize(emptyBoxIndex, goalBoxIndex_1, goalBoxIndex_2, clearBoxIndex_1, clearBoxIndex_2);

}

/*--------------------------------------------------
更新
戻り値	:次のシーン番号
--------------------------------------------------*/
GAME_SCENE PlayScene::Update(const DX::StepTimer& timer)
{
	// キー入力情報を取得する
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();
	keyStateTracker.Update(keyState);

	//[tab]キーが押されていたらポーズフラグを切り替える
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

	//ポーズフラグがfalseならゲームを更新する
	if (!m_pauseFlag)
	{
		PlayUpdate(timer);
	}	

	//フェードアウトを行う
	if (m_pBox->GetChangeScene())
	{
		if (!m_fadeFlag)
		{
			m_fadeFlag = true;
		}
	}

	//フェードアウトが完了したらシーンを移動する
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
void PlayScene::Draw()
{
	using namespace DirectX::SimpleMath;

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//ウィンドウサイズの取得
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	//変換行列の宣言(ワールド、ビュー、射影(プロジェクション))
	DirectX::SimpleMath::Matrix world, view, projection;

	//ワールド行列の初期化(単位行列)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pDebugCamera->GetViewMatrix();						//デバッグカメラを使う

	//プロジェクション
	constexpr float field0fView = DirectX::XMConvertToRadians(45.0f);	//画角
	float aspectRatio = width / height;                         //画面縦横比
	float nearPlane = 1.0f;										//カメラから一番近い投影面
	float farPlane = 200.0f;									//カメラから一番遠い投影面
	projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView
	(field0fView, aspectRatio, nearPlane, farPlane);

	m_projection = projection;
	
	//グリッドの表示
	//m_pGridFloor->Draw(context, m_commonState.get(), view, projection);

	//天球の描画
	m_pBackModel->UpdateEffects([&](DirectX::IEffect* effect)
		{
			DirectX::IEffectLights* lights = dynamic_cast<DirectX::IEffectLights*>(effect);
			if (lights)
			{
				// ライトの影響をなくす
				lights->SetAmbientLightColor(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
				lights->SetLightEnabled(0, false);
				lights->SetLightEnabled(1, false);
				lights->SetLightEnabled(2, false);
			}
			DirectX::BasicEffect* basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (basicEffect)
			{
				// エミッション色を設定する
				basicEffect->SetEmissiveColor(m_lightColor);
			}
		});
	world *= Matrix::CreateRotationZ(0.707f);
	m_pBackModel->Draw(context, *m_commonState.get(), world, view, projection);

	//ボックスの描画
	world = DirectX::SimpleMath::Matrix::Identity;
	m_pBox->Render(m_pDebugCamera.get());

	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//数字画像のサイズを取得
	Vector2 numTextSize = resourceManager.GetTextureSize(eTextureID::NUMBER);
	//数字画像を切り抜く時の幅
	long numRectWidth = (long)numTextSize.x / 11;

	//TODO::計算を別の関数で行う
	//経過時間の描画
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

	

	//スプライトバッチによる描画＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	m_spriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred, 
		m_commonState->NonPremultiplied()
	);

	//経過時間の描画
	DrawNum(m_timePos, numRect[m_seconds % 10]); //秒：1の位
	DrawNum(Vector2(m_timePos.x - (50 * 1), m_timePos.y), numRect[m_seconds / 10] ); //秒：10の位
	DrawNum(Vector2(m_timePos.x - (50 * 2), m_timePos.y), numRect[10]); //「：」
	DrawNum(Vector2(m_timePos.x - (50 * 3), m_timePos.y), numRect[m_minutes % 10]); //分：1の位
	DrawNum(Vector2(m_timePos.x - (50 * 4), m_timePos.y), numRect[m_minutes / 10]);//分：10の位
	DrawNum(Vector2(m_timePos.x - (50 * 5), m_timePos.y), numRect[10]); //「：」
	DrawNum(Vector2(m_timePos.x - (50 * 6), m_timePos.y), numRect[m_hours % 10]); //時：1の位
	DrawNum(Vector2(m_timePos.x - (50 * 7), m_timePos.y), numRect[m_hours / 10]); //時：10の位
	
	//移動回数の描画
	DrawNum(m_moveCountPos,numRect[(m_moveCount % 100) % 10]);//1の位
	DrawNum(Vector2(m_moveCountPos.x - (50 * 1), m_moveCountPos.y), numRect[(m_moveCount % 100) / 10]); //10の位
	DrawNum(Vector2(m_moveCountPos.x - (50 * 2), m_moveCountPos.y), numRect[(m_moveCount / 100)]); //100の位
	
	//moveCount文字位置
	DirectX::SimpleMath::Vector2 moveCountPos(-25, 0);
	//moveCount文字の描画
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

	//操作UIの描画
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
		//ポーズ時の描画をする
		//背景を半透明な黒色にする
		DirectX::XMVECTOR pauseBackColor = { 0.5f, 0.5f, 0.5f, 0.5f };
		//フェード用画像を背景色として利用
		m_spriteBatch->Draw(
			m_fadeImage.Get(),
			Vector2::Zero,
			nullptr,
			pauseBackColor
		);

		//ウィンドウの真ん中位置
		DirectX::SimpleMath::Vector2 centerPos(width / 2.0f, height / 2.0f);
		//説明書の描画
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
		//ポーズ画面終了するときのtabキー操作のUI
		m_spriteBatch->Draw(
			m_tabCloseImage.Get(),
			tabPos,
			nullptr,
			DirectX::Colors::White
		);
	}
	else
	{
		//[tab]キーの操作UIを描画する
		m_spriteBatch->Draw(
			m_tabImage.Get(),
			tabPos,
			nullptr,
			DirectX::Colors::White
		);
	}
	//フェード用の色と透明度を指定
	DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f, m_fadeAlpha };
	//フェード用画像の描画
	m_spriteBatch->Draw(
		m_fadeImage.Get(),
		Vector2::Zero,
		nullptr,
		color
	);
	//描画を終了する
	m_spriteBatch->End();
}

/*--------------------------------------------------
終了処理
--------------------------------------------------*/
void PlayScene::Finalize()
{
	//音を解放する
	m_pAdx2->Finalize();
	m_pBox->Finalize();
}

void PlayScene::PlayUpdate(const DX::StepTimer& timer)
{
	using namespace DirectX::SimpleMath;

	//経過時間
	float time = static_cast<float>(timer.GetElapsedSeconds());
	m_time += time;

	//当たり判定を行う準備＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//カメラの更新
	m_pDebugCamera->Update();

	//	ウィンドウサイズの取得
	int width = pDR->GetOutputSize().right;
	int height = pDR->GetOutputSize().bottom;

	//	マウス入力情報を取得する
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();
	m_mouseStateTracker.Update(mouseState);

	//フルスクリーン時の拡縮率
	RECT rect;
	GetClientRect(pDR->GetHwnd(), &rect); //左上を原点とした画面の相対座標を取得
	//拡縮率の計算
	m_scalingRatio = Game::SCREEN_H / static_cast<float>(rect.bottom);

	//マウスカーソルのスクリーン座標
	//フルスクリーン時に対応するためスクリーン拡縮の倍率を掛ける
	float mouseX = static_cast<float>(mouseState.x) * m_scalingRatio;
	float mouseY = static_cast<float>(mouseState.y) * m_scalingRatio;
	//マウスカーソルの位置
	DirectX::SimpleMath::Vector3 nearMousePos = DirectX::SimpleMath::Vector3(mouseX, mouseY, 0.0f);
	DirectX::SimpleMath::Vector3 farMousePos = DirectX::SimpleMath::Vector3(mouseX, mouseY, 1.0f);
	//ワールド座標への変換
	DirectX::SimpleMath::Matrix view = m_pDebugCamera->GetViewMatrix();
	DirectX::SimpleMath::Matrix revMat = CreateMatrix_Screen2World(width, height, view, m_projection);
	//レイ情報の作成
	DirectX::SimpleMath::Vector3 rayStart = DirectX::SimpleMath::Vector3::Transform(nearMousePos, revMat);
	DirectX::SimpleMath::Vector3 rayEnd = DirectX::SimpleMath::Vector3::Transform(farMousePos, revMat);
	//レイに見立てたカプセルの用意
	m_mouseRay.bigin = rayStart;
	m_mouseRay.end = rayEnd;
	m_mouseRay.radius = 0.0f;

	//ボックスの更新
	m_pBox->Update(
		timer,
		m_mouseStateTracker,
		m_mouseRay,
		m_pDebugCamera.get(),
		m_scalingRatio
	);

	//経過時間の更新
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

	//背景色を変える
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

	//移動回数を取得する
	m_moveCount = m_pBox->GetMovingCount();
	//音を取得する
	m_soundID = m_pBox->GetSoundID();
	//サウンドIDによって音を変える
	if (m_soundID == BoxManager::SoundID::MOVE)
	{
		//動いた時
		m_pAdx2->Play(CRI_CUESHEET_0_ISMOVESE);
	}
	else if (m_soundID == BoxManager::SoundID::NOT_MOVE)
	{
		//動かせなかった時
		m_pAdx2->Play(CRI_CUESHEET_0_NOTMOVESE);
	}
}



//============================================================================
//ワールド座標への変換用行列の生成
//============================================================================
DirectX::SimpleMath::Matrix PlayScene::CreateMatrix_Screen2World(
	int screen_w,
	int screen_h,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix projection
)
{
	//ビューポートスケーリング行列を作成
	DirectX::SimpleMath::Matrix viewport;
	viewport._11 = screen_w / 2.0f;
	viewport._22 = -screen_h / 2.0f;
	viewport._41 = screen_w / 2.0f;
	viewport._42 = screen_h / 2.0f;

	//逆行列を作成
	DirectX::SimpleMath::Matrix invS = viewport.Invert();
	DirectX::SimpleMath::Matrix invP = projection.Invert();
	DirectX::SimpleMath::Matrix invV = view.Invert();

	//『ビューポートスケーリング行列の逆行列』×『射影行列の逆行列』×『ビュー行列の逆行列』
	return invS * invP * invV;
}

//============================================================================
//数字を描画する
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