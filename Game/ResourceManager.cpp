/*
	ResourceManager.cpp
	������F2023.04.18
	����ҁF������T��
*/
#include "pch.h"
#include "ResourceManager.h"

//============================================================================
// �e�N�X�`���̐���
//============================================================================
bool ResourceManager::CreateTextures()
{
	//�ꎞ�I�ȉ摜�t�@�C�����X�g
	std::wstring fileList[] =
	{
		L"Resources/Textures/NextScene.png",	//�V�[���؂�ւ��L�[
		L"Resources/Textures/NextPlay.png",		//�V�[���؂�ւ��N���b�N
		L"Resources/Textures/Fade.png",			//�t�F�[�h
		L"Resources/Textures/Explanation.png",	//������
		L"Resources/Textures/Back.png",			//�w�i
		L"Resources/Textures/Title.png",		//�^�C�g�����S
		L"Resources/Textures/Number.png",		//����
		L"Resources/Textures/MoveCount.png",	//MoveCount����
		L"Resources/Textures/Operation.png",	//����UI
		L"Resources/Textures/Tab.png",			//TAB����
		L"Resources/Textures/TabClose.png",		//TAB����I��
		L"Resources/Textures/star.png",			//���G�t�F�N�g
		L"Resources/Textures/Frame.png",		//�{�b�N�X�̊O�g
		L"Resources/Textures/ClearFrame1.png",	//�N���A�����{�b�N�X�̊O�g1
		L"Resources/Textures/CrearFrame2.png",	//�N���A�����{�b�N�X�̊O�g2
		L"Resources/Textures/GoalFrame1.png",	//�S�[���{�b�N�X�̊O�g1
		L"Resources/Textures/GoalFrame2.png",	//�S�[���{�b�N�X�̊O�g2
		L"Resources/Textures/Clear.png",		//�N���A����
		L"Resources/Textures/Confetti.png",		//������
		L"Resources/Textures/ClearTime.png",	//ClearTime����
	
	};

	//�V�F�[�_���\�[�X�r���[�ƃ��\�[�X
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	//�e�N�X�`����ǂݍ���
	ID3D11Device1* d3dDevice = DX::DeviceResources::GetInstance()->GetD3DDevice();
	int num = static_cast<int>(eTextureID::NUM_TEX);
	HRESULT hr = E_FAIL;
	for (int i = 0; i < num; i++)
	{
		//�e�N�X�`�����쐬����
		hr = DirectX::CreateWICTextureFromFile(
			d3dDevice,
			fileList[i].c_str(),
			&resource,
			texture.ReleaseAndGetAddressOf()
		);
		assert(hr == S_OK);

		//�e�N�X�`����o�^����
		eTextureID state = static_cast<eTextureID>(i);
		m_textures[state] = texture;

		//�e�N�X�`���T�C�Y���v�Z����
		this->CalcTextureSize(resource, state);
	}

	//���肷��
	return true;
}

//============================================================================
// �e�N�X�`���̃T�C�Y���v�Z
//============================================================================
void ResourceManager::CalcTextureSize(
	const Microsoft::WRL::ComPtr<ID3D11Resource>& resource, 
	const eTextureID& id)
{
	ID3D11Texture2D* pTexture;
	D3D11_TEXTURE2D_DESC texDesc;

	pTexture = static_cast<ID3D11Texture2D*>(resource.Get());
	pTexture -> GetDesc(&texDesc);

	DirectX::SimpleMath::Vector2 texSize{
		static_cast<float>(texDesc.Width),
		static_cast<float>(texDesc.Height)
	};
	m_resources[id] = texSize;
}
