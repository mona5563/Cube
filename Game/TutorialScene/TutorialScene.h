/*
�`���[�g���A���V�[��
*/
#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

#include "../IScene.h"
#include "Game/GameMain.h"

//���֘A
#include "Libraries/ADX2/Adx2.h"
#include "../../SoundsHelpers/CueSheet_0.h"
#include "../../SoundsHelpers/CubeSound_acf.h"

class TutorialScene : public IScene
{
public:
	//�R���X�g���N�^
	TutorialScene();
	//�f�X�g���N�^
	~TutorialScene();
	//����������
	void Initialize() override;
	//�X�V����
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	//�`�悷��
	void Draw() override;
	//�I����������
	void Finalize() override;

private:
	//���֘A
	ADX2* m_pAdx2;

	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �X�v���C�g�t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_explanationImage; //������
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_nextImage; //�}�E�X�N���b�N
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage; //�t�F�[�h�p

	//�w�i�ʒu
	DirectX::SimpleMath::Vector2 m_backPosition;
	//�_�ŗp�^�C�}�[
	float m_alphaTime;
	//�_�ŗp�t���O
	bool m_isRender;
	//�t�F�[�h�̃A���t�@�l
	float m_fadeAlpha;
	//�t�F�[�h�p�t���O
	bool m_fadeFlag;
};