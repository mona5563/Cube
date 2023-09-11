//--------------------------------------------------------------------------------------
// File: Adx2.h
//
// �T�E���h�N���X
//
// Date: 2019.1.13
// Author: Hideyasu Imase
//
// �������ȒP�Ȏg�p���@������
//
// �V���O���g���N���X�Ȃ̂ŁwADX2::GetInstance()�x���g�p���ăC���X�^���X���擾���Ă��������B
// acf�t�@�C����acb�t�@�C�������g�p���܂���B�i�X�g���[�~���O�Đ��͖��Ή��j
//
// ���L�֐���g�ݍ���ŉ������B
//	�E������		Initialize(acf�t�@�C����, acb�t�@�C����)
//	�E�X�V����		Update()
//	�E�I������		Finalize()
//
// �EPlay()�ŉ����Đ�������߂�l��id���g�p����Stop�֐��Ŏ~�߂Ă��������B
// �ESetVolume()�͑S�̂̃{�����[����ݒ肵�܂��B
//
// �� �K�v�ŏ����̋@�\�������̃N���X�ɂ͂���܂���B�K�v�ɉ����ĉ������Ă��������B
// �� �K��.exe�Ɠ����t�H���_�Ɂwcri_ware_pcx86_le.dll�x�����Ă��������B
//
//--------------------------------------------------------------------------------------
#pragma once

//	�v���W�F�N�g�̃����N�ݒ肪��肭�ł��Ă��Ȃ��ƁA
//	�����ŃG���[�ɂȂ�\���A���I
#include <cri_adx2le.h>
//	Singleton.h�t�@�C�����A�����Ƃ��̃A�h���X�ɂ��邩�H
#include "../MyLibraries/Singleton.h"

class ADX2 : public Singleton<ADX2>
{
private:
	CriAtomExVoicePoolHn m_voice_pool;   /* �{�C�X�v�[���n���h�� */
	CriAtomDbasId m_dbas;                /* D-BAS�n���h�� */
	CriAtomExAcbHn m_acb_hn;             /* ACB�n���h�� */
	CriAtomExPlayerHn m_player;          /* �v���[���n���h�� */

public:
	// ������
	void Initialize(const char* acf, const char* acb);

	// �I������
	void Finalize();

	// �X�V����
	void Update();

	// �Đ�
	int Play(int no);

	// ��~
	void Stop(int id);

	// ���ʂ̐ݒ�֐�
	void SetVolume(float volume);

private:
	/* �G���[�R�[���o�b�N�֐��̃��[�U���� */
	static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray);

	/* �������m�ۊ֐��̃��[�U���� */
	static void *user_alloc_func(void *obj, CriUint32 size);

	/* ����������֐��̃��[�U���� */
	static void user_free_func(void *obj, void *ptr);
};