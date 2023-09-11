/*
���U���g�V�[��
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

class ResultScene : public IScene
{
public:
	//�N���A���Ԃ̐ݒ�
	void SetSecond(const int& second) { m_seconds = second; } //�b
	void SetMinute(const int& minute) { m_minutes = minute; } //��
	void SetHour(const int& hour) { m_hours = hour; } //����
	//�ړ���
	void SetMoveCount(const int& moveCount) { m_moveCount = moveCount; }

public:
	//�R���X�g���N�^
	ResultScene();
	//�f�X�g���N�^
	~ResultScene();
	//����������
	void Initialize() override;
	//�X�V����
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	//�`�悷��
	void Draw() override;
	//�I����������
	void Finalize() override;
	
private:
	//�����̕`�������
	void DrawNum(
		DirectX::SimpleMath::Vector2 pos,
		const RECT rect
	);
	//���E�ɐU��ʂ̗������o��
	float Swing();

private:
	//���֘A
	ADX2* m_pAdx2;
	int m_bgmID;

	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �X�v���C�g�t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearImage;		 //clear
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_confetti;		 //�����ᕗ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_nextSceneImage;	 //click
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backImage;		 //�w�i
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage;		 //�t�F�[�h�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_numTexture;		 //������
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearTimeTexture; //clearTime
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_moveCountTexture; //moveCount

	//�`��ʒu	
	DirectX::SimpleMath::Vector2 m_backPos;			//�w�i
	DirectX::SimpleMath::Vector2 m_clearPos;		//�N���A����
	DirectX::SimpleMath::Vector2 m_nextSceneImgPos;	//click
	DirectX::SimpleMath::Vector2 m_clearTimePos;	//�N���A�^�C��
	DirectX::SimpleMath::Vector2 m_clearTimeTexPos;	//clearTime����
	DirectX::SimpleMath::Vector2 m_moveCountPos;	//�ړ���
	DirectX::SimpleMath::Vector2 m_moveCountTexPos; //moveCount����
	DirectX::SimpleMath::Vector2 m_confettiPos;		//������
	//�t�F�[�h�̃A���t�@�l
	float m_fadeAlpha;
	//�t�F�[�h�t���O
	bool m_fadeFlag;
	//�N���A����
	int m_seconds;//�b
	int m_minutes;//��
	int m_hours;//����
	//�ړ���
	int m_moveCount;
	//�}�E�X����̕ۑ�
	DirectX::Mouse::ButtonStateTracker m_mouseStateTracker;
	//�e�N�X�`���T�C�Y
	DirectX::SimpleMath::Vector2 m_clearImageSize; //clear
	DirectX::SimpleMath::Vector2 m_nextImageSize;  //click
};