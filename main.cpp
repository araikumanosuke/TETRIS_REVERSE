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

#define GAME_SOUND_BGM_TITLE		"SOUND\\bgm_title_etc.mp3"
#define GAME_SOUND_BGM_PLAY			"SOUND\\bgm_play.mp3"
#define GAME_SOUND_BGM_END_CLEAR	"SOUND\\bgm_end_clear.mp3"
#define GAME_SOUND_BGM_END_OVER		"SOUND\\bgm_end_over.mp3"

#define GAME_IMAGE_BG_TITLE				"IMAGE\\bg_title.png"
#define GAME_IMAGE_BG_RANK				"IMAGE\\bg_rank.png"
#define GAME_IMAGE_BG_PLAY_ENDLESS		"IMAGE\\bg_play_endless.png"
#define GAME_IMAGE_BG_PLAY_TIME			"IMAGE\\bg_play_time.png"
#define GAME_IMAGE_BG_END_OVER			"IMAGE\\bg_end_over.png"
#define GAME_IMAGE_BG_END_CLEAR			"IMAGE\\bg_end_clear.png"

#define GAME_IMAGE_PLAY_BLOCK_RED		"IMAGE\\red.png"
#define GAME_IMAGE_PLAY_BLOCK_ORANGE	"IMAGE\\orange.png"
#define GAME_IMAGE_PLAY_BLOCK_YELLOW	"IMAGE\\yellow.png"
#define GAME_IMAGE_PLAY_BLOCK_GREEN		"IMAGE\\green.png"
#define GAME_IMAGE_PLAY_BLOCK_RIGHTBLUE	"IMAGE\\rightblue.png"
#define GAME_IMAGE_PLAY_BLOCK_BLUE		"IMAGE\\blue.png"
#define GAME_IMAGE_PLAY_BLOCK_PURPLE	"IMAGE\\purple.png"

#define MINO_KIND	7

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

enum BLOCK {
	RED,
	ORANGE,
	YELLOW,
	GREEN,
	RIGHTBLUE,
	BLUE,
	PURPLE
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

int syoki_flag;			//プレイ画面の初期処理で使う
//bool ground_flag;		//接地したか
bool firsthold_flag;	//最初のHOLDかどうか
bool holdfinish_flag;	//接地するまでにHOLDを2回以上使われないためのフラグ
int hold_taihi;			//HOLDミノ入れ替え処理で使用

bool flg_r;
bool flg_o;
bool flg_y;
bool flg_g;
bool flg_rb;
bool flg_b;
bool flg_p;

int mino_rand;
int nextmino_rand;

int holdmino;

int deleteline;
int reverseline;
int clearline;
int score;

int ready_fonthandle;
int Go_fonthandle;
int line_fonthandle;
int score_fonthandle;

int stage_move[18][10] =
{
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

int stage_put[18][10] =
{
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

int nextzone[4][3] =
{
	{-1,-1,-1},
	{-1,-1,-1},
	{-1,-1,-1},
	{-1,-1,-1}
};

int holdzone[4][3] =
{
	{-1,-1,-1},
	{-1,-1,-1},
	{-1,-1,-1},
	{-1,-1,-1}
};

SOUND bgm_title_etc;
SOUND bgm_play;
SOUND bgm_end_clear;
SOUND bgm_end_over;

IMAGE bg_title;
IMAGE bg_rank;
IMAGE bg_end_over;
IMAGE bg_end_clear;
IMAGE bg_play_endless;
IMAGE bg_play_time;

IMAGE block_red;
IMAGE block_orange;
IMAGE block_yellow;
IMAGE block_green;
IMAGE block_rightblue;
IMAGE block_blue;
IMAGE block_purple;

LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);	//自作ウィンドウプロシージャ

VOID MY_FPS_UPDATE(VOID);	//FPSを計測、更新する関数
VOID MY_FPS_WAIT(VOID);		//FPS値を計測し、待つ関数

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//キーの入力状態を更新する関数

VOID MY_SOUND_LOAD(SOUND *, const char *);
VOID MY_IMAGE_LOAD(IMAGE *, int, int, const char *);

VOID MOVE_BLOCK(IMAGE *, BLOCK);
VOID HOLD(VOID);

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

	if (DxLib_Init() == -1) { return -1; }	//DxLib初期化処理

	MY_SOUND_LOAD(&bgm_title_etc, GAME_SOUND_BGM_TITLE);
	MY_SOUND_LOAD(&bgm_play, GAME_SOUND_BGM_PLAY);
	MY_SOUND_LOAD(&bgm_end_clear, GAME_SOUND_BGM_END_CLEAR);
	MY_SOUND_LOAD(&bgm_end_over, GAME_SOUND_BGM_END_OVER);

	MY_IMAGE_LOAD(&bg_title, 0, 0, GAME_IMAGE_BG_TITLE);
	MY_IMAGE_LOAD(&bg_rank, 0, 0, GAME_IMAGE_BG_RANK);
	MY_IMAGE_LOAD(&bg_play_endless, 0, 0, GAME_IMAGE_BG_PLAY_ENDLESS);
	MY_IMAGE_LOAD(&bg_play_time, 0, 0, GAME_IMAGE_BG_PLAY_TIME);
	MY_IMAGE_LOAD(&bg_end_over, 0, 0, GAME_IMAGE_BG_END_OVER);
	MY_IMAGE_LOAD(&bg_end_clear, 0, 0, GAME_IMAGE_BG_END_CLEAR);

	MY_IMAGE_LOAD(&block_red, 0, 0, GAME_IMAGE_PLAY_BLOCK_RED);
	MY_IMAGE_LOAD(&block_orange, 0, 0, GAME_IMAGE_PLAY_BLOCK_ORANGE);
	MY_IMAGE_LOAD(&block_yellow, 0, 0, GAME_IMAGE_PLAY_BLOCK_YELLOW);
	MY_IMAGE_LOAD(&block_green, 0, 0, GAME_IMAGE_PLAY_BLOCK_GREEN);
	MY_IMAGE_LOAD(&block_rightblue, 0, 0, GAME_IMAGE_PLAY_BLOCK_RIGHTBLUE);
	MY_IMAGE_LOAD(&block_blue, 0, 0, GAME_IMAGE_PLAY_BLOCK_BLUE);
	MY_IMAGE_LOAD(&block_purple, 0, 0, GAME_IMAGE_PLAY_BLOCK_PURPLE);

	ready_fonthandle = CreateFontToHandle("UD デジタル 教科書体 NK-B", 60, -1, DX_FONTTYPE_ANTIALIASING);
	Go_fonthandle = CreateFontToHandle("UD デジタル 教科書体 NK-B", 120, -1, DX_FONTTYPE_ANTIALIASING);
	score_fonthandle = CreateFontToHandle("UD デジタル 教科書体 NK-B", 40, -1, DX_FONTTYPE_ANTIALIASING);
	line_fonthandle = CreateFontToHandle("UD デジタル 教科書体 NK-B", 48, -1, DX_FONTTYPE_ANTIALIASING);

	ChangeFont("UD デジタル 教科書体 NK-B");
	ChangeFontType(DX_FONTTYPE_ANTIALIASING);

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

	DeleteSoundMem(bgm_title_etc.handle);
	DeleteSoundMem(bgm_play.handle);
	DeleteSoundMem(bgm_end_clear.handle);
	DeleteSoundMem(bgm_end_over.handle);

	DeleteFontToHandle(ready_fonthandle);
	DeleteFontToHandle(Go_fonthandle);
	DeleteFontToHandle(score_fonthandle);
	DeleteFontToHandle(line_fonthandle);

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

VOID MOVE_BLOCK(IMAGE *i, BLOCK c)
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (stage_move[y][x] == c || stage_put[y][x] == c)
			{
				DrawGraph(x * 30 + 130, y * 30 + 75, i->handle, TRUE);
			}
		}
	}
	return;
}

VOID PUT_BLOCK(BLOCK c)
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (stage_move[y][x] == c)
			{
				stage_put[y][x] == c;
			}
		}
	}
}

VOID HOLD(VOID)
{
	if (AllKeyState[KEY_INPUT_SPACE] != 0)
	{
		if (holdfinish_flag == false)
		{
			if (firsthold_flag == true)
			{
				holdmino = mino_rand;
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				firsthold_flag = false;
				holdfinish_flag = true;
			}
			else
			{
				hold_taihi = holdmino;
				holdmino = mino_rand;
				mino_rand = hold_taihi;
				holdfinish_flag = true;
			}
		}
	}
	return;
}
VOID MY_GAME_TITLE(VOID)
{
	if (CheckSoundMem(bgm_title_etc.handle) == 0)
	{
		//PlaySoundMem(bgm_title_etc.handle, DX_PLAYTYPE_LOOP);
	}

	DrawGraph(bg_title.x, bg_title.y, bg_title.handle, TRUE);

	if (AllKeyState[KEY_INPUT_E] != 0)
	{
		syoki_flag = 0;

		//ground_flag = false;		//接地したか
		firsthold_flag = true;		//最初のHOLDかどうか
		holdfinish_flag = false;	//接地する前にHOLDが一度済んでいるか
		
		flg_r = true;
		flg_o = true;
		flg_y = true;
		flg_g = true;
		flg_rb = true;
		flg_b = true;
		flg_p = true;

		deleteline = 0;
		reverseline = 40;
		clearline = 200;
		score = 0;

		mino_rand = -1;
		nextmino_rand = -1;
		holdmino = -1;

		//StopSoundMem(bgm_title_etc.handle);
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}
	else if (AllKeyState[KEY_INPUT_T] != 0)
	{
		//StopSoundMem(bgm_title_etc.handle);
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
	if (CheckSoundMem(bgm_title_etc.handle) == 0)
	{
		//PlaySoundMem(bgm_title_etc.handle, DX_PLAYTYPE_LOOP);
	}

	DrawGraph(bg_rank.x, bg_rank.y, bg_rank.handle, TRUE);

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	return;
}

VOID MY_GAME_SOZAI(VOID)
{
	if (CheckSoundMem(bgm_title_etc.handle) == 0)
	{
		//PlaySoundMem(bgm_title_etc.handle, DX_PLAYTYPE_LOOP);
	}

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
	/*初期処理*/
	DrawGraph(bg_play_endless.x, bg_play_endless.y, bg_play_endless.handle, TRUE);

	DrawFormatStringToHandle(37, 320, GetColor(255, 100, 0), line_fonthandle, "%d", reverseline);
	DrawFormatStringToHandle(476, 320, GetColor(0, 0, 255), line_fonthandle, "%d", deleteline);
	DrawFormatStringToHandle(395, 25, GetColor(255, 255, 255), score_fonthandle, "%d", score);

	static int syoki_temp;

	static int tmp_r;
	static int tmp_o;
	static int tmp_y;
	static int tmp_g;
	static int tmp_rb;
	static int tmp_b;
	static int tmp_p;

	int cnt_r;
	int cnt_o;
	int cnt_y;
	int cnt_g;
	int cnt_rb;
	int cnt_b;
	int cnt_p;

	int syoki_count = GetNowCount();

	if (syoki_flag == 0)
	{
		syoki_temp = syoki_count;
		syoki_flag = 1;
	}

	if (syoki_count - syoki_temp < 1000)
	{
		mino_rand = GetRand(MINO_KIND - 1);
		nextmino_rand = GetRand(MINO_KIND - 1);

		DrawStringToHandle(150, 250, "Ready...", GetColor(255, 255, 255), ready_fonthandle);
	}
	else if (syoki_count - syoki_temp > 1001 && syoki_count - syoki_temp < 2000)
	{
		DrawStringToHandle(170, 400, "Go!", GetColor(255, 255, 255), Go_fonthandle);
	}
	else if (syoki_count - syoki_temp >= 2000)
	{
		syoki_count = syoki_temp + 2000;

		if (CheckSoundMem(bgm_play.handle) == 0)
		{
			//PlaySoundMem(bgm_play.handle, DX_PLAYTYPE_LOOP);
		}

		MOVE_BLOCK(&block_red, RED);
		MOVE_BLOCK(&block_orange, ORANGE);
		MOVE_BLOCK(&block_yellow, YELLOW);
		MOVE_BLOCK(&block_green, GREEN);
		MOVE_BLOCK(&block_rightblue, RIGHTBLUE);
		MOVE_BLOCK(&block_blue, BLUE);
		MOVE_BLOCK(&block_purple, PURPLE);

		//操作するミノ
		switch (mino_rand)
		{
		case RED:
			cnt_r = GetNowCount();
			if (flg_r == true)
			{
				tmp_r = cnt_r;
				flg_r = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 17 && cnt < 17000; x_move++, cnt += 1000)
			{
				if (cnt_r - tmp_r > cnt && cnt_r - tmp_r <= cnt + 1000)
				{
					HOLD();

					if (cnt_r - tmp_r > 1000)
					{
						stage_move[x_move - 1][4] = -1;
						stage_move[x_move - 1][5] = -1;
						stage_move[x_move][5] = -1;
						stage_move[x_move][6] = -1;
					}

					stage_move[x_move][4] = RED;
					stage_move[x_move][5] = RED;
					stage_move[x_move + 1][5] = RED;
					stage_move[x_move + 1][6] = RED;
				}
			}
					
			if (cnt_r - tmp_r == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_r = true;
				cnt_r = 0;
				tmp_r = 0;
			}

			break;

		case ORANGE:
			cnt_o = GetNowCount();
			if (flg_o == true)
			{
				tmp_o = cnt_o;
				flg_o = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 17 && cnt < 17000; x_move++, cnt += 1000)
			{
				if (cnt_o - tmp_o > cnt && cnt_o - tmp_o <= cnt + 1000)
				{
					HOLD();

					if (cnt_o - tmp_o > 1000)
					{
						stage_move[x_move - 1][6] = -1;
						stage_move[x_move][4] = -1;
						stage_move[x_move][5] = -1;
						stage_move[x_move][6] = -1;
					}

					stage_move[x_move][6] = ORANGE;
					stage_move[x_move + 1][4] = ORANGE;
					stage_move[x_move + 1][5] = ORANGE;
					stage_move[x_move + 1][6] = ORANGE;
				}
			}

			if (cnt_o - tmp_o == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_o = true;
				cnt_o = 0;
				tmp_o = 0;
			}
			break;

		case YELLOW:
			cnt_y = GetNowCount();
			if (flg_y == true)
			{
				tmp_y = cnt_y;
				flg_y = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 17 && cnt < 17000; x_move++, cnt += 1000)
			{
				if (cnt_y - tmp_y > cnt && cnt_y - tmp_y <= cnt + 1000)
				{
					HOLD();

					if (cnt_y - tmp_y > 1000)
					{
						stage_move[x_move - 1][4] = -1;
						stage_move[x_move - 1][5] = -1;
						stage_move[x_move][4] = -1;
						stage_move[x_move][5] = -1;
					}

					stage_move[x_move][4] = YELLOW;
					stage_move[x_move][5] = YELLOW;
					stage_move[x_move + 1][4] = YELLOW;
					stage_move[x_move + 1][5] = YELLOW;
				}
			}

			if (cnt_y - tmp_y == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_y = true;
				cnt_y = 0;
				tmp_y = 0;
			}
			break;

		case GREEN:
			cnt_g = GetNowCount();
			if (flg_g == true)
			{
				tmp_g = cnt_g;
				flg_g = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 17 && cnt < 17000; x_move++, cnt += 1000)
			{
				if (cnt_g - tmp_g > cnt && cnt_g - tmp_g <= cnt + 1000)
				{
					HOLD();
					
					if (cnt_g - tmp_g > 1000)
					{
						stage_move[x_move - 1][5] = -1;
						stage_move[x_move - 1][6] = -1;
						stage_move[x_move][4] = -1;
						stage_move[x_move][5] = -1;
					}

					stage_move[x_move][5] = GREEN;
					stage_move[x_move][6] = GREEN;
					stage_move[x_move + 1][4] = GREEN;
					stage_move[x_move + 1][5] = GREEN;
				}
			}

			if (cnt_g - tmp_g == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_g = true;
				cnt_g = 0;
				tmp_g = 0;
			}
			break;

		case RIGHTBLUE:
			cnt_rb = GetNowCount();
			if (flg_rb == true)
			{
				tmp_rb = cnt_rb;
				flg_rb = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 18 && cnt < 18000; x_move++, cnt += 1000)
			{
				if (cnt_rb - tmp_rb > cnt && cnt_rb - tmp_rb <= cnt + 1000)
				{
					HOLD();

					if (cnt_rb - tmp_rb > 1000)
					{
						stage_move[x_move - 1][3] = -1;
						stage_move[x_move - 1][4] = -1;
						stage_move[x_move - 1][5] = -1;
						stage_move[x_move - 1][6] = -1;
					}

					stage_move[x_move][3] = RIGHTBLUE;
					stage_move[x_move][4] = RIGHTBLUE;
					stage_move[x_move][5] = RIGHTBLUE;
					stage_move[x_move][6] = RIGHTBLUE;
				}
			}

			if (cnt_rb - tmp_rb == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_rb = true;
				cnt_rb = 0;
				tmp_rb = 0;
			}
			break;

		case BLUE:
			cnt_b = GetNowCount();
			if (flg_b == true)
			{
				tmp_b = cnt_b;
				flg_b = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 17 && cnt < 17000; x_move++, cnt += 1000)
			{
				if (cnt_b - tmp_b > cnt && cnt_b - tmp_b <= cnt + 1000)
				{
					HOLD();

					if (cnt_b - tmp_b > 1000)
					{
						stage_move[x_move - 1][4] = -1;
						stage_move[x_move][4] = -1;
						stage_move[x_move][5] = -1;
						stage_move[x_move][6] = 1;
					}

					stage_move[x_move][4] = BLUE;
					stage_move[x_move + 1][4] = BLUE;
					stage_move[x_move + 1][5] = BLUE;
					stage_move[x_move + 1][6] = BLUE;
				}
			}
			if (cnt_b - tmp_b == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_b = true;
				cnt_b = 0;
				tmp_b = 0;
			}
			break;

		case PURPLE:
			cnt_p = GetNowCount();
			if (flg_p == true)
			{
				tmp_p = cnt_p;
				flg_p = false;
			}

			for (int x_move = 0, cnt = 0; x_move < 17 && cnt < 17000; x_move++, cnt += 1000)
			{
				if (cnt_p - tmp_p > cnt && cnt_p - tmp_p <= cnt + 1000)
				{
					HOLD();

					if (cnt_p - tmp_p > 1000)
					{
						stage_move[x_move - 1][5] = -1;
						stage_move[x_move][4] = -1;
						stage_move[x_move][5] = -1;
						stage_move[x_move][6] = 1;
					}

					stage_move[x_move][5] = PURPLE;
					stage_move[x_move + 1][4] = PURPLE;
					stage_move[x_move + 1][5] = PURPLE;
					stage_move[x_move + 1][6] = PURPLE;
				}
			}

			if (cnt_p - tmp_p == 17000)
			{
				mino_rand = nextmino_rand;
				nextmino_rand = GetRand(MINO_KIND - 1);
				holdfinish_flag = false;
				flg_p = true;
				cnt_p = 0;
				tmp_p = 0;
			}
			break;
		}

		//次に操作するミノ
		switch (nextmino_rand)
		{
		case RED:
			nextzone[1][0] = RED;
			nextzone[1][1] = RED;
			nextzone[2][1] = RED;
			nextzone[2][2] = RED;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == RED)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_red.handle, TRUE);
					}
				}
			}
			break;

		case ORANGE:
			nextzone[1][2] = ORANGE;
			nextzone[2][0] = ORANGE;
			nextzone[2][1] = ORANGE;
			nextzone[2][2] = ORANGE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == ORANGE)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_orange.handle, TRUE);
					}
				}
			}
			break;

		case YELLOW:
			nextzone[1][0] = YELLOW;
			nextzone[1][1] = YELLOW;
			nextzone[2][0] = YELLOW;
			nextzone[2][1] = YELLOW;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == YELLOW)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_yellow.handle, TRUE);
					}
				}
			}
			break;

		case GREEN:
			nextzone[1][1] = GREEN;
			nextzone[1][2] = GREEN;
			nextzone[2][0] = GREEN;
			nextzone[2][1] = GREEN;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == GREEN)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_green.handle, TRUE);
					}
				}
			}
			break;

		case RIGHTBLUE:
			nextzone[0][1] = RIGHTBLUE;
			nextzone[1][1] = RIGHTBLUE;
			nextzone[2][1] = RIGHTBLUE;
			nextzone[3][1] = RIGHTBLUE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == RIGHTBLUE)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_rightblue.handle, TRUE);
					}
				}
			}
			break;

		case BLUE:
			nextzone[1][0] = BLUE;
			nextzone[2][0] = BLUE;
			nextzone[2][1] = BLUE;
			nextzone[2][2] = BLUE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == BLUE)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_blue.handle, TRUE);
					}
				}
			}
			break;

		case PURPLE:
			nextzone[1][1] = PURPLE;
			nextzone[2][0] = PURPLE;
			nextzone[2][1] = PURPLE;
			nextzone[2][2] = PURPLE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (nextzone[y][x] == PURPLE)
					{
						DrawGraph(x * 30 + 445, y * 30 + 75, block_purple.handle, TRUE);
					}
				}
			}
			break;
		}

		//HOLDしておくミノ
		switch (holdmino)
		{
		case RED:
			holdzone[1][0] = RED;
			holdzone[1][1] = RED;
			holdzone[2][1] = RED;
			holdzone[2][2] = RED;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == RED)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_red.handle, TRUE);
					}
				}
			}
			break;

		case ORANGE:
			holdzone[1][2] = ORANGE;
			holdzone[2][0] = ORANGE;
			holdzone[2][1] = ORANGE;
			holdzone[2][2] = ORANGE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == ORANGE)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_orange.handle, TRUE);
					}
				}
			}
			break;

		case YELLOW:
			holdzone[1][0] = YELLOW;
			holdzone[1][1] = YELLOW;
			holdzone[2][0] = YELLOW;
			holdzone[2][1] = YELLOW;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == YELLOW)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_yellow.handle, TRUE);
					}
				}
			}
			break;

		case GREEN:
			holdzone[1][1] = GREEN;
			holdzone[1][2] = GREEN;
			holdzone[2][0] = GREEN;
			holdzone[2][1] = GREEN;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == GREEN)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_green.handle, TRUE);
					}
				}
			}
			break;

		case RIGHTBLUE:
			holdzone[0][1] = RIGHTBLUE;
			holdzone[1][1] = RIGHTBLUE;
			holdzone[2][1] = RIGHTBLUE;
			holdzone[3][1] = RIGHTBLUE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == RIGHTBLUE)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_rightblue.handle, TRUE);
					}
				}
			}
			break;

		case BLUE:
			holdzone[1][0] = BLUE;
			holdzone[2][0] = BLUE;
			holdzone[2][1] = BLUE;
			holdzone[2][2] = BLUE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == BLUE)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_blue.handle, TRUE);
					}
				}
			}
			break;

		case PURPLE:
			holdzone[1][1] = PURPLE;
			holdzone[2][0] = PURPLE;
			holdzone[2][1] = PURPLE;
			holdzone[2][2] = PURPLE;

			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 3; x++)
				{
					if (holdzone[y][x] == PURPLE)
					{
						DrawGraph(x * 30 + 25, y * 30 + 75, block_purple.handle, TRUE);
					}
				}
			}
			break;
		}

	}

	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		//StopSoundMem(bgm_play.handle);
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
	DrawGraph(bg_play_time.x, bg_play_time.y, bg_play_time.handle, TRUE);

	if (CheckSoundMem(bgm_play.handle) == 0)
	{
		//PlaySoundMem(bgm_play.handle, DX_PLAYTYPE_LOOP);
	}

	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		//StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_END_OVER;
	}
	else if (AllKeyState[KEY_INPUT_SPACE] != 0)
	{
		//StopSoundMem(bgm_play.handle);
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
	SetFontSize(24);
	DrawString(0, 0, "確認画面(エンドレス)", GetColor(255, 255, 255));
	DrawString(0, 30, "ゲームを終了しますか？", GetColor(255, 255, 255));
	DrawString(0, 60, "タイトル：Ｙキー", GetColor(255, 255, 255));
	DrawString(0, 90, "プレイを続行：Ｎキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		//StopSoundMem(bgm_play.handle);
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
	SetFontSize(24);
	DrawString(0, 0, "確認画面(エンドレス)", GetColor(255, 255, 255));
	DrawString(0, 30, "ゲームを終了しますか？", GetColor(255, 255, 255));
	DrawString(0, 60, "タイトル：Ｙキー", GetColor(255, 255, 255));
	DrawString(0, 90, "プレイを続行：Ｎキー", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		//StopSoundMem(bgm_play.handle);
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
	if (CheckSoundMem(bgm_end_over.handle) == 0)
	{
		//PlaySoundMem(bgm_end_over.handle, DX_PLAYTYPE_LOOP);
	}

	DrawGraph(bg_end_over.x, bg_end_over.y, bg_end_over.handle, TRUE);

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		//StopSoundMem(bgm_end_over.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		//StopSoundMem(bgm_end_over.handle);
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}

	return;
}

VOID MY_GAME_END_CLEAR(VOID)
{
	if (CheckSoundMem(bgm_end_clear.handle) == 0)
	{
		//PlaySoundMem(bgm_end_clear.handle, DX_PLAYTYPE_LOOP);
	}

	DrawGraph(bg_end_clear.x, bg_end_clear.y, bg_end_clear.handle, TRUE);

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		//StopSoundMem(bgm_end_clear.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		//StopSoundMem(bgm_end_clear.handle);
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