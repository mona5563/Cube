//
// GameMain.h
//
#pragma once

#include <Keyboard.h>
#include <Mouse.h>
#include "StepTimer.h"

//�V�[���̗�
enum class GAME_SCENE : int
{
    NONE,

    TITLE,
    TUTORIAL,
    PLAY,
    RESULT,
};

//�O���錾
class DebugCamera;
class IScene;

/// <summary>
/// Game�N���X����A���[�U���������𔲂��o�����N���X
/// </summary>
class GameMain
{
public:
    //�R���X�g���N�^
    GameMain();
    //�f�X�g���N�^
    ~GameMain();

    //����������
    void Initialize();
    //�X�V����
    void Update(const DX::StepTimer& timer);
    //�`�悷��
    void Render();
    //�I������������
    void Finalize();

    // �V�[���֌W����

    // �V�[���̍쐬
    void CreateScene();

    // �V�[���̍폜
    void DeleteScene();

private:

    //�L�[�{�[�h
    std::unique_ptr<DirectX::Keyboard> m_keybord;
    //�}�E�X
    std::unique_ptr<DirectX::Mouse> m_mouse;
    //���̃V�[���ԍ�
	GAME_SCENE m_nextScene;
	//�V�[��
	std::unique_ptr<IScene> m_pScene;

    //�N���A���ԗp�ϐ�
    int m_seconds;
    int m_minutes;
    int m_hours;

    //�ړ���
    int m_moveNum;



    
};
