/*
�v���C�V�[��
*/
#pragma once

#include <CommonStates.h>
#include <GeometricPrimitive.h>
#include<Model.h>

#include "../IScene.h"
#include "Game/GameMain.h"

#include "Box/BoxManager.h"

//���֘A
#include "Libraries/ADX2/Adx2.h"
#include "../../SoundsHelpers/CueSheet_0.h"
#include "../../SoundsHelpers/CubeSound_acf.h"

class GridFloor;
class Camera;
class CapsuleCollider;

class PlayScene : public IScene
{
public:
	//�o�ߎ��Ԃ��擾����
	int GetSeconds() const { return m_seconds; }
	int GetMinutes() const { return m_minutes; }
	int GetHours() const { return m_hours; }
	//�ړ��񐔂��擾����
	int GetMoveCount() const { return m_moveCount; }

public:
	//�R���X�g���N�^
	PlayScene();
	//�f�X�g���N�^
	~PlayScene();
	//����������
	void Initialize() override;
	//�X�V����
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	//�`�悷��
	void Draw() override;
	//�I����������
	void Finalize() override;
	//�Q�[�����X�V����
	void PlayUpdate(const DX::StepTimer& timer);

private:
	//	�X�N���[���̑傫����view�Eprojection�s�񂩂�A���[���h���W�֕ϊ����邽�߂̍s��𐶐�����
	DirectX::SimpleMath::Matrix CreateMatrix_Screen2World(
		int screen_w,
		int screen_h,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix projection
	);
	//�����̕`�������
	void DrawNum(
		DirectX::SimpleMath::Vector2 pos,
		const RECT rect
	);

private:
	//���֘A
	ADX2* m_pAdx2;
	int m_bgmID;
	BoxManager::SoundID m_soundID;

	//�R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	//�X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//�X�v���C�g�t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	//�f�o�b�O�J����
	std::unique_ptr<DebugCamera> m_pDebugCamera;
	//�O���b�h��
	std::unique_ptr <GridFloor> m_pGridFloor;

	//�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_numTexture; //UI�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fadeImage; //�t�F�[�h�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_moveCountTexture; //moveCount
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_operationTexture; //����UI
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_explanationImage; //������
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tabImage; //tab
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tabCloseImage; //tabClose
	
	//�V��
	std::unique_ptr<DirectX::Model> m_pBackModel; //���f��
	DirectX::SimpleMath::Color m_lightColor; //���C�g�̐F
	float m_colorTimer; //���`��ԗp�^�C�}�[
	//�{�b�N�X
	std::unique_ptr<BoxManager> m_pBox;
	//�}�E�X�̑���̕ۑ�
	DirectX::Mouse::ButtonStateTracker m_mouseStateTracker;
	//�L�[�{�[�h�̑���̕ۑ�
	DirectX::Keyboard::KeyboardStateTracker keyStateTracker;
	//�����蔻��p�}�E�X���C
	Capsule m_mouseRay;
	//�R���C�_�[
	std::unique_ptr<CapsuleCollider> m_collider;
	//projection���o���Ă������߂̕ϐ�
	DirectX::SimpleMath::Matrix m_projection;
	//�o�ߎ���
	float m_time;
	int m_seconds;//�b
	int m_minutes;//��
	int m_hours;//����
	//�o�ߎ��ԕ`��ʒu
	DirectX::SimpleMath::Vector2 m_timePos;
	//�t�F�[�h�̃A���t�@�l
	float m_fadeAlpha;
	//�t�F�[�h�t���O
	bool m_fadeFlag;
	//�ړ���
	int m_moveCount;
	//�ړ��񐔕`��ʒu
	DirectX::SimpleMath::Vector2 m_moveCountPos;
	//��ʊg�k��
	float m_scalingRatio; 
	//�|�[�Y�t���O
	bool m_pauseFlag; 
};