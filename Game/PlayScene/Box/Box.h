//
//Box.h
//
#pragma once

#include <GeometricPrimitive.h>
#include <SimpleMath.h>

#include "GameDate/Objects.h"

//�O���錾
class Camera;

//�{�b�N�X�N���X
class Box
{
public:
	//�{�b�N�X�̏��
	enum class State
	{
		STAY,		//�����Ă��Ȃ�
		MOVING,		//�����Ă���
	};
	//�{�b�N�X�̃^�C�v
	enum class Type
	{
		NORMAL,		//�����{�b�N�X
		CLEAR_1,	//1�ƕ`���ꂽ�N���A�p�{�b�N�X
		CLEAR_2		//2�ƕ`���ꂽ�N���A�p�{�b�N�X
	};

public:
	//�ʒu���擾����
	const DirectX::SimpleMath::Vector3& GetPosition() const;
	//�ʒu��ݒ肷��
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	//�F���擾����
	const DirectX::SimpleMath::Color& GetColor() const;
	//�F��ݒ肷��
	void SetColor(const DirectX::SimpleMath::Color& color);
	//�X�^�[�g�ʒu���擾����
	DirectX::SimpleMath::Vector3 GetStartPos() const;
	//�X�^�[�g�ʒu��ݒ肷��
	void SetStartPos(const DirectX::SimpleMath::Vector3& startPos);
	//�S�[���ʒu���擾����
	DirectX::SimpleMath::Vector3 GetGoalPos() const;
	//�S�[���ʒu��ݒ肷��
	void SetGoalPos(const DirectX::SimpleMath::Vector3& goalPos);
	//Type���擾����
	Type GetType() { return m_type; }
	// ��Ԃ��擾����
	const State& GetCurrentState() const
	{
		return m_state;
	}
	// ��Ԃ�ݒ肷��
	void SetCurrentState(const State& state)
	{
		m_state = state;
	}
	//�J�v�Z�����擾����
	Capsule GetCapsule() const { return m_hitCapsule; }

public:
	//�R���X�g���N�^
	Box();
	//�f�X�g���N�^
	~Box();
	//����������
	void Initialize(const DirectX::SimpleMath::Vector3& cupsulePos, Type type);
	//�X�V����
	void Update(const DX::StepTimer& timer, float scalingRatio);
	//�`�悷��
	void Render(const Camera* pCamera);
	//�㏈������
	void Finalize();

	//���`���
	float Lerp(
		const float& start,
		const float& end,
		const float& t
	);
	//�C�[�W���O�֐�
	float easeOutBounce(const float& x);
	float easeInBounce(const float& x);

	////�J�v�Z���ƃJ�v�Z���̏Փ˔��������
	//float ClosestPtSegmentSegment(
	//	const DirectX::SimpleMath::Vector3& mouseBegin,
	//	const DirectX::SimpleMath::Vector3& mouseEnd,
	//	float& s,
	//	float& t,
	//	DirectX::SimpleMath::Vector3& c1,
	//	DirectX::SimpleMath::Vector3& c2
	//);
	////Clamp�֐�
	//float Clamp(const float& n, const float& min, const float& max);
	////2�̐����̍ŒZ�����̕�����Ԃ�
	//bool  HitCheck_Capsule2Capsule(const Capsule& mouse);

private:
	//�ړ��ɂ����鎞��(�萔)
	static const float MOVE_TIME;

private:
	//�W�I���g���b�N�v���~�e�B�u
	std::unique_ptr<DirectX::GeometricPrimitive> m_pBox;
	//�ʒu
	DirectX::SimpleMath::Vector3 m_position;
	//�F
	DirectX::SimpleMath::Color m_color;
	//�����蔻��p�̃��C(�J�v�Z��)
	Capsule m_hitCapsule;
	//�q�b�g��Ԃ��L�^����t���O
	float m_hit;
	//�ړ�����
	float m_moveTime;
	//�o�ߎ���
	float m_time;
	// ���
	State m_state;
	//�^�C�v
	Type  m_type;
	//�X�^�[�g�ʒu
	DirectX::SimpleMath::Vector3  m_startPos;
	//�S�[���ʒu
	DirectX::SimpleMath::Vector3  m_goalPos;
	//���[�`��
	int m_boxRoutine;
	//�W�I���g���b�N�v���~�e�B�u�ɒ���t����e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_frame; //�O�g
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearframe_1; //1�̂�������
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearframe_2; //2�̂�������
	//���`��Ԏ��̈ʒu
	DirectX::SimpleMath::Vector3 m_renderPos;
};
 