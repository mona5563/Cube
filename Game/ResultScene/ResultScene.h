/*
リザルトシーン
*/
#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

#include "../IScene.h"
#include "Game/GameMain.h"

//音関連
#include "Libraries/ADX2/Adx2.h"
#include "../../SoundsHelpers/CueSheet_0.h"
#include "../../SoundsHelpers/CubeSound_acf.h"

class ResultScene : public IScene
{
public:
	//クリア時間の設定
	void SetSecond(const int& second) { m_seconds = second; } //秒
	void SetMinute(const int& minute) { m_minutes = minute; } //分
	void SetHour(const int& hour) { m_hours = hour; } //時間
	//移動回数
	void SetMoveCount(const int& moveCount) { m_moveCount = moveCount; }

public:
	//コンストラクタ
	ResultScene();
	//デストラクタ
	~ResultScene();
	//初期化する
	void Initialize() override;
	//更新する
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	//描画する
	void Draw() override;
	//終了処理する
	void Finalize() override;
	
private:
	//数字の描画をする
	void DrawNum(
		DirectX::SimpleMath::Vector2 pos,
		const RECT rect
	);
	//左右に振る量の乱数を出す
	float Swing();

private:
	//音関連
	ADX2* m_pAdx2;
	int m_bgmID;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// スプライトフォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearImage;		 //clear
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_confetti;		 //紙吹雪風
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_nextSceneImage;	 //click
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backImage;		 //背景
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage;		 //フェード用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_numTexture;		 //数字類
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearTimeTexture; //clearTime
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_moveCountTexture; //moveCount

	//描画位置	
	DirectX::SimpleMath::Vector2 m_backPos;			//背景
	DirectX::SimpleMath::Vector2 m_clearPos;		//クリア文字
	DirectX::SimpleMath::Vector2 m_nextSceneImgPos;	//click
	DirectX::SimpleMath::Vector2 m_clearTimePos;	//クリアタイム
	DirectX::SimpleMath::Vector2 m_clearTimeTexPos;	//clearTime文字
	DirectX::SimpleMath::Vector2 m_moveCountPos;	//移動回数
	DirectX::SimpleMath::Vector2 m_moveCountTexPos; //moveCount文字
	DirectX::SimpleMath::Vector2 m_confettiPos;		//紙吹雪
	//フェードのアルファ値
	float m_fadeAlpha;
	//フェードフラグ
	bool m_fadeFlag;
	//クリア時間
	int m_seconds;//秒
	int m_minutes;//分
	int m_hours;//時間
	//移動回数
	int m_moveCount;
	//マウス操作の保存
	DirectX::Mouse::ButtonStateTracker m_mouseStateTracker;
	//テクスチャサイズ
	DirectX::SimpleMath::Vector2 m_clearImageSize; //clear
	DirectX::SimpleMath::Vector2 m_nextImageSize;  //click
};