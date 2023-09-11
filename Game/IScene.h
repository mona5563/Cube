/*
�V�[���C���^�[�t�F�[�X
*/
#pragma once

#include "StepTimer.h"
#include "GameMain.h"


class IScene
{
public:
	//�f�X�g���N�^
	virtual ~IScene() = default;
	//����������
	virtual void Initialize() = 0;
	//�X�V����
	virtual GAME_SCENE Update(const DX::StepTimer& timer) = 0;
	//�`�悷��
	virtual void Draw() = 0;
	//�I����������
	virtual void Finalize() = 0;
};