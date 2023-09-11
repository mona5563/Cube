#pragma once

#include "Box.h"
#include "Game/Shader/EffectManager.h"
#include "GameDate/Collision.h"

struct Capsule;
class DebugCamera;
class CapsuleCollider;

class BoxManager
{
public:
	//�{�b�N�X�̏��
	enum class State
	{
		STAY,
		BOX_MOVING,
		FINISH
	};

	//SE
	enum class SoundID
	{
		MOVE,
		NOT_MOVE,
		NONE,
	};

public:
	//�ׂ��󂢂Ă��邩�擾����
	bool GetVicinityNull() const { return m_isVicinityNull; }
	//�ׂ��S�[���Ȃ̂��擾����
	bool GetVicinityClear() const { return m_isVicinityClear; }
	//�ړ������擾����
	int GetMovingCount() const { return m_moveCount; }
	//���ʉ����擾����
	SoundID GetSoundID() const { return m_soundID; }
	//�V�[���J�ڃt���O���擾����
	bool GetChangeScene() const { return m_changeScene; }

public:
	//�R���X�g���N�^
	BoxManager();
	//�f�X�g���N�^
	~BoxManager();
	//����������
	void Initialize(
		const int emptyBoxIndex,
		const int goalBoxIndex_1,
		const int goalBoxIndex_2,
		const int clearBoxIndex_1,
		const int clearBoxIndex_2
	);
	//�X�V����
	void Update(
		const DX::StepTimer& timer,
		DirectX::Mouse::ButtonStateTracker mouseStateTracker,
		Capsule mouseRay,
		DebugCamera* camera,
		float scalingRatio
	);
	//�`�悷��
	void Render(DebugCamera* camera);
	//�I����������
	void Finalize();
	//�{�b�N�X�̈ꕔ���폜����
	void Delete(int element);
	//�ׂ��󂢂Ă��邩���肷��
	bool VicinityNullPos(
		const DirectX::SimpleMath::Vector3&  nullPos,
		const DirectX::SimpleMath::Vector3& boxPos
	);
	//�ׂ��N���A�����̃{�b�N�X�Ȃ̂����肷��
	bool VicinityClearPos(
		const DirectX::SimpleMath::Vector3& colorBoxPos,
		const DirectX::SimpleMath::Vector3& clearPos
	);

private:
	//�{�b�N�X
	std::vector<std::unique_ptr<Box>> m_pBox;
	//�F
	DirectX::SimpleMath::Color m_color;
	//�����Ă���t���O
	Box* m_movingBox;
	//�󂢂Ă���{�b�N�X�̏ꏊ
	DirectX::SimpleMath::Vector3  m_nullPos;
	//�{�b�N�X�̏��
	State m_state;
	//�N���A�ƂȂ�{�b�N�X�̈ʒu
	DirectX::SimpleMath::Vector3 m_clearBox1Pos;
	DirectX::SimpleMath::Vector3 m_clearBox2Pos;
	//�O�g�̃e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_goalframe_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_goalframe_2;
	//�S�[���ƂȂ�{�b�N�X�̈ʒu
	DirectX::SimpleMath::Vector3 m_goalBox1Pos;
	DirectX::SimpleMath::Vector3 m_goalBox2Pos;
	//�N���A�ƂȂ�{�b�N�X�̃W�I���g���b�N�v���~�e�B�u
	std::unique_ptr<DirectX::GeometricPrimitive> m_pGoalBox1;
	std::unique_ptr<DirectX::GeometricPrimitive> m_pGoalBox2;
	//���S�e�p�{�b�N�X
	std::unique_ptr<DirectX::GeometricPrimitive> m_pShadowBox;
	//�V�[���ύX�p�t���O
	bool m_changeScene;
	//����`�悷�邩�̃t���O
	bool m_isRender;
	//�ׂ��󂢂��ꏊ���̃t���O
	bool m_isVicinityNull;
	//�N���A�����ƂȂ�{�b�N�X�ƃN���A�{�b�N�X�̃t���O
	bool m_isVicinityClear;
	//�ړ���
	int m_moveCount;
	//��
	SoundID m_soundID;
	//���̃G�t�F�N�g
	std::unique_ptr<EffectManager> m_boxEffect;
	//�R���C�_�[
	std::unique_ptr<CapsuleCollider> m_collider;
};