//--------------------------------------------------------------------------------------
// File: Adx2.h
//
// サウンドクラス
//
// Date: 2019.1.13
// Author: Hideyasu Imase
//
// ★★★簡単な使用方法★★★
//
// シングルトンクラスなので『ADX2::GetInstance()』を使用してインスタンスを取得してください。
// acfファイルとacbファイルしか使用しません。（ストリーミング再生は未対応）
//
// 下記関数を組み込んで下さい。
//	・初期化		Initialize(acfファイル名, acbファイル名)
//	・更新処理		Update()
//	・終了処理		Finalize()
//
// ・Play()で音を再生したら戻り値のidを使用してStop関数で止めてください。
// ・SetVolume()は全体のボリュームを設定します。
//
// ※ 必要最小限の機能しかこのクラスにはありません。必要に応じて改造してください。
// ※ 必ず.exeと同じフォルダに『cri_ware_pcx86_le.dll』を入れてください。
//
//--------------------------------------------------------------------------------------
#pragma once

//	プロジェクトのリンク設定が上手くできていないと、
//	ここでエラーになる可能性アリ！
#include <cri_adx2le.h>
//	Singleton.hファイルが、ちゃんとこのアドレスにあるか？
#include "../MyLibraries/Singleton.h"

class ADX2 : public Singleton<ADX2>
{
private:
	CriAtomExVoicePoolHn m_voice_pool;   /* ボイスプールハンドル */
	CriAtomDbasId m_dbas;                /* D-BASハンドル */
	CriAtomExAcbHn m_acb_hn;             /* ACBハンドル */
	CriAtomExPlayerHn m_player;          /* プレーヤハンドル */

public:
	// 初期化
	void Initialize(const char* acf, const char* acb);

	// 終了処理
	void Finalize();

	// 更新処理
	void Update();

	// 再生
	int Play(int no);

	// 停止
	void Stop(int id);

	// 音量の設定関数
	void SetVolume(float volume);

private:
	/* エラーコールバック関数のユーザ実装 */
	static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray);

	/* メモリ確保関数のユーザ実装 */
	static void *user_alloc_func(void *obj, CriUint32 size);

	/* メモリ解放関数のユーザ実装 */
	static void user_free_func(void *obj, void *ptr);
};