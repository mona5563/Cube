/*
プレイシーン
*/
#pragma once

#include <CommonStates.h>
#include <GeometricPrimitive.h>
#include<Model.h>

#include "../IScene.h"
#include "Game/GameMain.h"

#include "Box/BoxManager.h"

//音関連
#include "Libraries/ADX2/Adx2.h"
#include "../../SoundsHelpers/CueSheet_0.h"
#include "../../SoundsHelpers/CubeSound_acf.h"

class GridFloor;
class Camera;
class CapsuleCollider;

class PlayScene : public IScene
{
public:
	//経過時間を取得する
	int GetSeconds() const { return m_seconds; }
	int GetMinutes() const { return m_minutes; }
	int GetHours() const { return m_hours; }
	//移動回数を取得する
	int GetMoveCount() const { return m_moveCount; }

public:
	//コンストラクタ
	PlayScene();
	//デストラクタ
	~PlayScene();
	//初期化する
	void Initialize() override;
	//更新する
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	//描画する
	void Draw() override;
	//終了処理する
	void Finalize() override;
	//ゲームを更新する
	void PlayUpdate(const DX::StepTimer& timer);

private:
	//	スクリーンの大きさやview・projection行列から、ワールド座標へ変換するための行列を生成する
	DirectX::SimpleMath::Matrix CreateMatrix_Screen2World(
		int screen_w,
		int screen_h,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix projection
	);
	//数字の描画をする
	void DrawNum(
		DirectX::SimpleMath::Vector2 pos,
		const RECT rect
	);

private:
	//音関連
	ADX2* m_pAdx2;
	int m_bgmID;
	BoxManager::SoundID m_soundID;

	//コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	//スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//スプライトフォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	//デバッグカメラ
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	//グリッド線
	std::unique_ptr <GridFloor> m_pGridFloor;

	//テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_numTexture; //UIテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage; //フェードテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_moveCountTexture; //moveCount
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_operationTexture; //操作UI
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_explanationImage; //説明書
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tabImage; //tab
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tabCloseImage; //tabClose
	
	//天球
	std::unique_ptr<DirectX::Model> m_pBackModel; //モデル
	DirectX::SimpleMath::Color m_lightColor; //ライトの色
	float m_colorTimer; //線形補間用タイマー
	//ボックス
	std::unique_ptr<BoxManager> m_pBox;
	//マウスの操作の保存
	DirectX::Mouse::ButtonStateTracker m_mouseStateTracker;
	//キーボードの操作の保存
	DirectX::Keyboard::KeyboardStateTracker keyStateTracker;
	//当たり判定用マウスレイ
	Capsule m_mouseRay;
	//コライダー
	std::unique_ptr<CapsuleCollider> m_collider;
	//projectionを覚えておくための変数
	DirectX::SimpleMath::Matrix m_projection;
	//経過時間
	float m_time;
	int m_seconds;//秒
	int m_minutes;//分
	int m_hours;//時間
	//経過時間描画位置
	DirectX::SimpleMath::Vector2 m_timePos;
	//フェードのアルファ値
	float m_fadeAlpha;
	//フェードフラグ
	bool m_fadeFlag;
	//移動回数
	int m_moveCount;
	//移動回数描画位置
	DirectX::SimpleMath::Vector2 m_moveCountPos;
	//画面拡縮率
	float m_scalingRatio; 
	//ポーズフラグ
	bool m_pauseFlag; 
};