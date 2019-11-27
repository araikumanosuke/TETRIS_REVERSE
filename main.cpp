#include "DxLib.h"

#define GAME_WIDTH	560
#define GAME_HEIGHT 625
#define GAME_COLOR	32

#define GAME_WINDOW_NAME		"TETRIS_REVERSE"
#define GAME_WINDOW_MODECHANGE	TRUE	//TRUE:ウィンドウモード　FALSE:フルスクリーン

#define SET_WINDOW_ST_MODE_DEFAULT			0
#define SET_WINDOW_ST_MODE_TITLE_NONE		1
#define SET_WINDOW_ST_MODE_TITLE_FLAME_NONE	2
#define SET_WINDOW_ST_MODE_FLAME_NONE		3

#define GAME_FPS_SPEED	60

#define GAME_SOUND_BGM_TITLE	"SOUND\\bgm_title.mp3"
#define GAME_SOUND_BGM_PLAY		"SOUND\\bgm_title_2.mp3"

#define GAME_IMAGE_BG_TITLE		"IMAGE\\title.png"


enum GAME_SCENE {
	GAME_SCENE_TITLE,			//タイトル画面
	GAME_SCENE_RANKING,			//ランキング画面
	GAME_SCENE_SOZAI,			//素材表示画面
	GAME_SCENE_PLAY_ENDLESS,	//プレイ画面(エンドレスモード)
	GAME_SCENE_PLAY_TIME,		//プレイ画面(タイムアタックモード)
	GAME_SCENE_CHECK_ENDLESS,	//確認画面
	GAME_SCENE_CHECK_TIME,		//確認画面
	GAME_SCENE_END_CLEAR,		//エンド画面(ゲームクリア)
	GAME_SCENE_END_OVER			//エンド画面(ゲームオーバー)
};


struct SOUND {
	int handle;
	char filepath[128];
	int playtype;
};

struct IMAGE {
	int handle;
	char filepath[128];
	int x;
	int y;
	int width;
	int height;
};

BOOL IsWM_CREATE = FALSE;	//WM_CREATEが正常に動作したか判断する

int StartTimeFps;					//測定開始時刻
int CountFps;						//カウンタ
float CalcFps;						//計算結果
int SampleNumFps = GAME_FPS_SPEED;	//平均を取るサンプル数

char AllKeyState[256];	//全てのキーの状態が入る

int GameSceneNow = (int)GAME_SCENE_TITLE;	//最初のゲーム画面をタイトルに設定

SOUND bgm_title;
SOUND bgm_play;

IMAGE bg_title;

LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);	//自作ウィンドウプロシージャ

VOID MY_FPS_UPDATE(VOID);	//FPSを計測、更新する関数
VOID MY_FPS_WAIT(VOID);		//FPS値を計測し、待つ関数

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//キーの入力状態を更新する関数

VOID MY_SOUND_LOAD(SOUND *,const char *);

VOID MY_IMAGE_LOAD(IMAGE *, int, int, const char *);

VOID MY_GAME_TITLE(VOID);			//タイトル
VOID MY_GAME_RANKING(VOID);			//ランキング
VOID MY_GAME_SOZAI(VOID);			//素材表示
VOID MY_GAME_PLAY_ENDLESS(VOID);	//プレイ・エンドレス
VOID MY_GAME_PLAY_TIME(VOID);		//プレイ・タイムアタック
VOID MY_GAME_CHECK_ENDLESS(VOID);	//確認
VOID MY_GAME_CHECK_TIME(VOID);		//確認
VOID MY_GAME_END_CLEAR(VOID);		//ゲームクリア
VOID MY_GAME_END_OVER(VOID);		//ゲームオーバー


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(GAME_WINDOW_MODECHANGE);	//ウィンドウモードに設定
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//指定の数値で画面を表示する

	SetWindowStyleMode(SET_WINDOW_ST_MODE_DEFAULT);	//タイトルバー

	SetMainWindowText(TEXT(GAME_WINDOW_NAME));		//タイトルの文字

	if (DxLib_Init() == -1)	{ return -1; }	//DxLib初期化処理

	MY_SOUND_LOAD(&bgm_title, GAME_SOUND_BGM_TITLE);
	MY_SOUND_LOAD(&bgm_play, GAME_SOUND_BGM_PLAY);

	MY_IMAGE_LOAD(&bg_title, 0, 0, GAME_IMAGE_BG_TITLE);

	SetDrawScreen(DX_SCREEN_BACK);	//Draw系関数は裏画面に描画

	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//メッセージ処理の結果がエラーの時、強制終了

		if (ClearDrawScreen() != 0) { break; }	//画面を消去出来なかった時、強制終了
	
		MY_ALL_KEYDOWN_UPDATE();

		switch (GameSceneNow)
		{
		case (int)GAME_SCENE_TITLE:
			MY_GAME_TITLE();
			break;

		case (int)GAME_SCENE_RANKING:
			MY_GAME_RANKING();
			break;

		case (int)GAME_SCENE_SOZAI:
			MY_GAME_SOZAI();
			break;

		case (int)GAME_SCENE_PLAY_ENDLESS:
			MY_GAME_PLAY_ENDLESS();
			break;

		case (int)GAME_SCENE_PLAY_TIME:
			MY_GAME_PLAY_TIME();
			break;

		case (int)GAME_SCENE_CHECK_ENDLESS:
			MY_GAME_CHECK_ENDLESS();
			break;

		case (int)GAME_SCENE_CHECK_TIME:
			MY_GAME_CHECK_TIME();
			break;

		case (int)GAME_SCENE_END_CLEAR:
			MY_GAME_END_CLEAR();
			break;

		case (int)GAME_SCENE_END_OVER:
			MY_GAME_END_OVER();
			break;

		default:
			break;
		}

		MY_FPS_UPDATE();	//FPSの処理(更新)

		ScreenFlip();		//モニタのリフレッシュレートの速さで裏画面を再描画
		
		MY_FPS_WAIT();		//FPSの処理(待つ)
	}

	DeleteSoundMem(bgm_title.handle);
	DeleteSoundMem(bgm_play.handle);

	DxLib_End();

	return 0;
}

//FPSを計測、更新する関数
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)
	{
		StartTimeFps = GetNowCount();	//測定開始時刻設定
	}

	if (CountFps == SampleNumFps)
	{
		int now = GetNowCount();		//現時刻を取得
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);	//計算
		CountFps = 0;	//カウントリセット
		StartTimeFps = now;	//現時刻から測定開始
	}
	CountFps++;
	return;
}

//FPS値を計測し、待つ関数
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;	//かかった時間
	int waitTime = CountFps * 1000 / GAME_FPS_SPEED - resultTime;	//待つべき時間

	//指定のFPS値よりも早い場合
	if (waitTime > 0)
	{
		Sleep(waitTime);	//待つ
	}
	return;
}

//キーの入力状態を更新する関数
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[256];	//一時的に現在のキーの入力状態を格納する

	GetHitKeyStateAll(TempKey);	//全てのキーの入力状態を得る

	for (int i = 0; i < 256; i++)	//押されているキーのキーコードを押しているとき
	{
		if (TempKey[i] != 0)	//押されている
		{
			AllKeyState[i]++;
		}
		else
		{
			AllKeyState[i] = 0;	//押されていない
		}
	}
	return;
}

VOID MY_SOUND_LOAD(SOUND *s, const char *path)
{
	s->handle = LoadSoundMem(path);
	return;
}

VOID MY_IMAGE_LOAD(IMAGE *i, int x, int y, const char *path)
{
	wsprintf(i->filepath, path);
	i->handle = LoadGraph(i->filepath);

	GetGraphSize(i->handle, &i->width, &i->height);
	i->x = x;
	i->y = y;

	return;
}

VOID MY_GAME_TITLE(VOID)
{
	if (CheckSoundMem(bgm_title.handle) == 0)
	{
		PlaySoundMem(bgm_title.handle, DX_PLAYTYPE_LOOP);
	}

	DrawGraph(bg_title.x, bg_title.y, bg_title.handle, TRUE);

	DrawBox(155, 400, 405, 520, GetColor(0, 0, 0), TRUE);

	DrawString(160, 405, "エンドレスモード：Ｅキー", GetColor(255, 255, 255));
	DrawString(160, 435, "タイムアタックモード：Ｔキー", GetColor(255, 255, 255));
	DrawString(160, 465, "ランキング：Ｒキー", GetColor(255, 255, 255));
	DrawString(160, 495, "素材：Ｓキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_E] != 0)
	{
		StopSoundMem(bgm_title.handle);
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}
	else if (AllKeyState[KEY_INPUT_T] != 0)
	{
		StopSoundMem(bgm_title.handle);
		GameSceneNow = (int)GAME_SCENE_PLAY_TIME;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}
	else if (AllKeyState[KEY_INPUT_S] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_SOZAI;
	}

	return;
}

VOID MY_GAME_RANKING(VOID)
{
	DrawString(0, 0, "ランキング画面", GetColor(255, 255, 255));
	DrawString(0, 30, "タイトルに戻る：ＢＡＣＫＳＰＡＣＥキー", GetColor(255, 255, 255));
	
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	return;
}

VOID MY_GAME_SOZAI(VOID)
{
	DrawString(0, 0, "素材表示画面", GetColor(255, 255, 255));
	DrawString(0, 30, "タイトルに戻る：ＢＡＣＫＳＰＡＣＥキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	return;
}

VOID MY_GAME_PLAY_ENDLESS(VOID)
{
	if (CheckSoundMem(bgm_play.handle) == 0)
	{
		PlaySoundMem(bgm_play.handle, DX_PLAYTYPE_LOOP);
	}

	DrawString(0, 0, "プレイ画面(エンドレスモード)", GetColor(255, 255, 255));
	DrawString(0, 30, "エンド画面：ＥＮＴＥＲキー", GetColor(255, 255, 255));
	DrawString(0, 60, "確認画面：ＢＡＣＫＳＰＡＣＥキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_END_OVER;
	}
	else if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_CHECK_ENDLESS;
	}

	return;
}

VOID MY_GAME_PLAY_TIME(VOID)
{
	if (CheckSoundMem(bgm_play.handle) == 0)
	{
		PlaySoundMem(bgm_play.handle, DX_PLAYTYPE_LOOP);
	}

	DrawString(0, 0, "プレイ画面(タイムアタックモード)", GetColor(255, 255, 255));
	DrawString(0, 30, "エンド画面(ゲームオーバー)：ＥＮＴＥＲキー", GetColor(255, 255, 255));
	DrawString(0, 60, "エンド画面(ゲームクリア)：ＳＰＡＣＥキー", GetColor(255, 255, 255));
	DrawString(0, 90, "確認画面：ＢＡＣＫＳＰＡＣＥキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_END_OVER;
	}
	else if (AllKeyState[KEY_INPUT_SPACE] != 0)
	{
		StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_END_CLEAR;
	}
	else if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_CHECK_TIME;
	}

	return;
}

VOID MY_GAME_CHECK_ENDLESS(VOID)
{
	DrawString(0, 0, "確認画面(エンドレス)", GetColor(255, 255, 255));
	DrawString(0, 30, "ゲームを終了しますか？", GetColor(255, 255, 255));
	DrawString(0, 60, "タイトル：Ｙキー", GetColor(255, 255, 255));
	DrawString(0, 90, "プレイを続行：Ｎキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_N] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}

	return;
}

VOID MY_GAME_CHECK_TIME(VOID)
{
	DrawString(0, 0, "確認画面(エンドレス)", GetColor(255, 255, 255));
	DrawString(0, 30, "ゲームを終了しますか？", GetColor(255, 255, 255));
	DrawString(0, 60, "タイトル：Ｙキー", GetColor(255, 255, 255));
	DrawString(0, 90, "プレイを続行：Ｎキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_N] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY_TIME;
	}

	return;
}

VOID MY_GAME_END_OVER(VOID)
{
	DrawString(0, 0, "エンド画面(ゲームオーバー)", GetColor(255, 255, 255));
	DrawString(0, 30, "タイトル：ＢＡＣＫＳＰＡＣＥキー", GetColor(255, 255, 255));
	DrawString(0, 60, "ランキング：Ｒキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}

	return;
}

VOID MY_GAME_END_CLEAR(VOID)
{
	DrawString(0, 0, "エンド画面(ゲームクリア)", GetColor(255, 255, 255));
	DrawString(0, 30, "タイトル：ＢＡＣＫＳＰＡＣＥキー", GetColor(255, 255, 255));
	DrawString(0, 60, "ランキング：Ｒキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}
	
	return;
}

LRESULT CALLBACK MY_WNDPROC(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CREATE:

		IsWM_CREATE = TRUE;
		return 0;

	case WM_CLOSE:

		break;

	case WM_RBUTTONDOWN:
		
		break;

	case WM_LBUTTONDOWN:
		PostMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, lp);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}