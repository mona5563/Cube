//
// GameMain.h
//
#pragma once

#include <Keyboard.h>
#include <Mouse.h>
#include "StepTimer.h"

//シーンの列挙
enum class GAME_SCENE : int
{
    NONE,

    TITLE,
    TUTORIAL,
    PLAY,
    RESULT,
};

//前方宣言
class DebugCamera;
class IScene;

/// <summary>
/// Gameクラスから、ユーザ処理部分を抜き出したクラス
/// </summary>
class GameMain
{
public:
    //コンストラクタ
    GameMain();
    //デストラクタ
    ~GameMain();

    //初期化する
    void Initialize();
    //更新する
    void Update(const DX::StepTimer& timer);
    //描画する
    void Render();
    //終了処理をする
    void Finalize();

    // シーン関係処理

    // シーンの作成
    void CreateScene();

    // シーンの削除
    void DeleteScene();

private:

    //キーボード
    std::unique_ptr<DirectX::Keyboard> m_keybord;
    //マウス
    std::unique_ptr<DirectX::Mouse> m_mouse;
    //次のシーン番号
	GAME_SCENE m_nextScene;
	//シーン
	std::unique_ptr<IScene> m_pScene;

    //クリア時間用変数
    int m_seconds;
    int m_minutes;
    int m_hours;

    //移動回数
    int m_moveNum;



    
};
