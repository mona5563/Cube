//-----------------------------------
//	�G�t�F�N�g�Ǘ��N���X	
//	EffectManager.h
//-----------------------------------
#pragma once

#include "StepTimer.h"

#include <SimpleMath.h>
#include <list>
#include <WICTextureLoader.h>

#include "Game/Shader/MyEffect.h"

class EffectManager
{
private:

	//�o�b�`�G�t�F�N�g
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;

	//�v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_batch;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//�G�t�F�N�g�̃|�C���^
	std::list<std::unique_ptr<MyEffect>>	 m_effectList;

	//�e�N�X�`���f�[�^
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_texture;

public:

	void Create();

	//Create()�őS�������͒����̂ŕ���
	void Initialize(float life, DirectX::SimpleMath::Vector3 pos);

	void Update(const DX::StepTimer& timer);

	void Render();

	void SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
};