/*
タイトルシーン
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

class TitleScene : public IScene
{
public:
	//コンストラクタ
	TitleScene();
	//デストラクタ
	~TitleScene();
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
	int m_bgmID;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// スプライトフォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleLogo; //タイトルロゴ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleBack; //背景
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_nextSceneImage; //click
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage; //フェード用

	//背景位置
	DirectX::SimpleMath::Vector2 m_backPosition;
	//フェードのアルファ値
	float m_fadeAlpha;
	//フェードフラグ
	bool m_fadeFlag;
	//マウス操作の保存
	DirectX::Mouse::ButtonStateTracker m_mouseStateTracker;
	//テクスチャサイズ
	DirectX::SimpleMath::Vector2 m_logoImageSize; //ロゴ
	DirectX::SimpleMath::Vector2 m_nextImageSize; //click
};