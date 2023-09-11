/*
	ResourceManager.h
	������F2023.04.18
	����ҁF������T��
*/

#pragma once
#include <unordered_map>

//�e�N�X�`��ID
enum class eTextureID
{
	NEXT_SCENE_KEY = 0,	//�V�[���؂�ւ��L�[
	NEXT_SCENE_CLICK,	//�V�[���؂�ւ��N���b�N
	FADE,				//�t�F�[�h
	EXPLANATION,		//������
	BACK,				//�w�i
	TITLE_LOGO,			//�^�C�g�����S
	NUMBER,				//����
	MOVE_COUNT,			//MoveCount����
	OPERATION,			//����UI
	TAB_OPEN,			//TAB����
	TAB_CLOSE,			//TAB����I��
	EFFECT_STAR,		//���G�t�F�N�g
	BOX_FRAME,			//�{�b�N�X�̊O�g
	CLEARBOX_FRAME_1,	//�N���A�����{�b�N�X�̊O�g1
	CLEARBOX_FRAME_2,	//�N���A�����{�b�N�X�̊O�g2
	GOALBOX_FRAME_1,	//�S�[���{�b�N�X�̊O�g1
	GOALBOX_FRAME_2,	//�S�[���{�b�N�X�̊O�g2
	CLEAR,				//�N���A����
	CONFETTI,			//������
	CLEAR_TIME,			//ClearTime����


	NUM_TEX,			//�e�N�X�`�����v��
};

class ResourceManager
{
public:
	//�C���X�^���X�̎擾
	static ResourceManager& GetRefInstance()
	{
		static ResourceManager instance;
		return instance;
	}

private:
	std::unordered_map<eTextureID,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;

	std::unordered_map<eTextureID, DirectX::SimpleMath::Vector2> m_resources;

private:
	//�R���X�g���N�^
	ResourceManager()
		: m_textures{}
	{
		this->CreateTextures();
	};

	//�f�X�g���N�^
	~ResourceManager() = default;

	//�R�s�[�R���X�g���N�^�A�R�s�[������Z�q�̍폜
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator = (const ResourceManager&) = delete;
	
	//���[�u�R���X�g���N�^�A���[�u������Z�q�̍폜
	ResourceManager(const ResourceManager&&) = delete;
	ResourceManager&& operator = (const ResourceManager&&) = delete;

public:
	//�e�N�X�`���̎擾
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const eTextureID& id)
	{
		return m_textures[id];
	}
	//�e�N�X�`���T�C�Y�̎擾
	const DirectX::SimpleMath::Vector2& GetTextureSize(const eTextureID& id)
	{
		return m_resources[id];
	}

private:
	//�e�N�X�`���̐���
	bool CreateTextures();

	//�e�N�X�`���̃T�C�Y���v�Z
	void CalcTextureSize(
		const Microsoft::WRL::ComPtr<ID3D11Resource>& resource,
		const eTextureID& id
	);
};
