/*
チュートリアルシーン
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

class TutorialScene : public IScene
{
public:
	//コンストラクタ
	TutorialScene();
	//デストラクタ
	~TutorialScene();
	//初期化する
	void Initialize() override;
	//更新する
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	//描画する
	void Draw() override;
	//終了処理する
	void Finalize() override;

private:
	//音関連
	ADX2* m_pAdx2;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// スプライトフォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_explanationImage; //説明書
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_nextImage; //マウスクリック
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage; //フェード用

	//背景位置
	DirectX::SimpleMath::Vector2 m_backPosition;
	//点滅用タイマー
	float m_alphaTime;
	//点滅用フラグ
	bool m_isRender;
	//フェードのアルファ値
	float m_fadeAlpha;
	//フェード用フラグ
	bool m_fadeFlag;
};