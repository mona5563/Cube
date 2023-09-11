//----------------------------------------
// MyEffect.h
//----------------------------------------
#pragma once

#include "StepTimer.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

class MyEffect
{
protected:

	DX::StepTimer m_timer;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	// エフェクト
	DirectX::AlphaTestEffect* m_batchEffect;

	// プリミティブバッチ
	DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>* m_batch;

	// 入力レイアウト
	ID3D11InputLayout* m_inputLayout;

	std::unique_ptr<DirectX::CommonStates> m_states;

	// テクスチャハンドル
	ID3D11ShaderResourceView* m_texture;

	DirectX::SimpleMath::Vector3		m_camera;

	DirectX::SimpleMath::Vector3		m_position;
	DirectX::SimpleMath::Vector3		m_velocity;

	float								m_life;

	DirectX::SimpleMath::Vector3		m_startPosition;
	DirectX::SimpleMath::Vector3		m_startVelocity;
	float								m_startLife;

	float								m_wait;

	DirectX::SimpleMath::Vector3        m_scale;

public:

	//マネージャ管理のため共通項目を外部から貰えるように変更
	void Create(ID3D11ShaderResourceView* texture, DirectX::AlphaTestEffect* batchEffect, DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>* batch, ID3D11InputLayout* inputLayout);

	// 初期化
	void Initialize(float life = 100.0f, DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3 velocity = DirectX::SimpleMath::Vector3::Zero);

	//リセット
	void Reset();

	// 更新
	void Update(DX::StepTimer timer);

	// 描画
	void Render();

	// 描画情報の設定
	void SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

protected:

	// 描画の詳細処理
	void Draw();
};