#include "pch.h"
#include "DeviceResources.h"
#include "EffectManager.h"

#include "Game/ResourceManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void EffectManager::Create()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	ID3D11Device1* device = pDR->GetD3DDevice();

	//バッチエフェクトの作成
	m_batchEffect = std::make_unique<AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);
	// 入力レイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf());

	//プリミティブバッチの作成
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(pDR->GetD3DDeviceContext());

	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();

	//各画像の取得
	m_texture = resourceManager.GetTexture(eTextureID::EFFECT_STAR);
	

	int count = 20;
	for (int i = 0; i < count; ++i)
	{
		std::unique_ptr<MyEffect> effect = std::make_unique<MyEffect>();
		effect->Create(m_texture.Get(), m_batchEffect.get(), m_batch.get(), m_inputLayout.Get());
		m_effectList.push_back(std::move(effect));
	}
}

void EffectManager::Initialize(float life, Vector3 pos)
{
	int range = 100;

	//life,pos,vel の値でm_effectを初期化する
	for (std::list<std::unique_ptr<MyEffect>>::iterator ite = m_effectList.begin();
		ite != m_effectList.end(); ++ite)
	{
		Vector3 vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f);
		while (vel.Length() < 0.03f)
		{
			vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f);
		}
		(*ite)->Initialize(life, pos, vel);
	}
}

void EffectManager::Update(const DX::StepTimer& timer)
{
	//timerを渡してm_effectの更新処理を行う
	for (std::list<std::unique_ptr<MyEffect>>::iterator ite = m_effectList.begin();
		ite != m_effectList.end(); ++ite)
	{
		(*ite)->Update(timer);
	}
}

void EffectManager::Render()
{
	//m_effectを描画する
	for (std::list<std::unique_ptr<MyEffect>>::iterator ite = m_effectList.begin();
		ite != m_effectList.end(); ++ite)
	{
		(*ite)->Render();
	}
}

void EffectManager::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	//camera,view,proj,の値をm_effectに渡す
	for (std::list<std::unique_ptr<MyEffect>>::iterator ite = m_effectList.begin();
		ite != m_effectList.end(); ++ite)
	{
		(*ite)->SetRenderState(camera, view, proj);;
	}
}
