#include "DxLib.h"

#define GAME_WIDTH	560
#define GAME_HEIGHT 625
#define GAME_COLOR	32

#define GAME_WINDOW_NAME		"TETRIS_REVERSE"
#define GAME_WINDOW_MODECHANGE	TRUE	//TRUE:�E�B���h�E���[�h�@FALSE:�t���X�N���[��

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
	GAME_SCENE_TITLE,			//�^�C�g�����
	GAME_SCENE_RANKING,			//�����L���O���
	GAME_SCENE_SOZAI,			//�f�ޕ\�����
	GAME_SCENE_PLAY_ENDLESS,	//�v���C���(�G���h���X���[�h)
	GAME_SCENE_PLAY_TIME,		//�v���C���(�^�C���A�^�b�N���[�h)
	GAME_SCENE_CHECK_ENDLESS,	//�m�F���
	GAME_SCENE_CHECK_TIME,		//�m�F���
	GAME_SCENE_END_CLEAR,		//�G���h���(�Q�[���N���A)
	GAME_SCENE_END_OVER			//�G���h���(�Q�[���I�[�o�[)
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

struct TIME {
	int min;	//��
	int sec;	//�b
	int m_sec;	//�~���b
};

BOOL IsWM_CREATE = FALSE;	//WM_CREATE������ɓ��삵�������f����

int StartTimeFps;					//����J�n����
int CountFps;						//�J�E���^
float CalcFps;						//�v�Z����
int SampleNumFps = GAME_FPS_SPEED;	//���ς����T���v����

char AllKeyState[256];	//�S�ẴL�[�̏�Ԃ�����

int GameSceneNow = (int)GAME_SCENE_TITLE;	//�ŏ��̃Q�[����ʂ��^�C�g���ɐݒ�

int syoki_flag;			//�v���C��ʂ̏��������Ŏg��
//bool ground_flag;		//�ڒn������
bool firsthold_flag;	//�ŏ���HOLD���ǂ���
bool holdfinish_flag;	//�ڒn����܂ł�HOLD��2��ȏ�g���Ȃ����߂̃t���O

int mino_rand;
int nextmino_rand;

int holdmino;

int deleteline;
int reverseline;
int clearline;
int score;

int tmp;	//�~�m�̎������� / �����㏸�ɂĎg�p

int stop_tmp;	//�m�F��ʂŎg��

/*�����L���O�Ŏg���ϐ�*/
static int score_No1 = 0;
static int score_No2 = 0;
static int score_No3 = 0;
static int score_No4 = 0;
static int score_No5 = 0;

static int deleteline_No1 = 0;
static int deleteline_No2 = 0;
static int deleteline_No3 = 0;
static int deleteline_No4 = 0;
static int deleteline_No5 = 0;

static TIME No1 = { 0,0,0 };
static TIME No2 = { 0,0,0 };
static TIME No3 = { 0,0,0 };
static TIME No4 = { 0,0,0 };
static TIME No5 = { 0,0,0 };
/*�����L���O�Ŏg���ϐ������܂�*/

bool reverse_flag;

bool first_flag;

bool play_flag;

int ready_fonthandle;
int Go_fonthandle;
int line_fonthandle;
int score_fonthandle;
int rank_fonthandle;

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

bool stage_put_flag[18][10]=
{
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false}
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

LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);	//����E�B���h�E�v���V�[�W��

VOID MY_FPS_UPDATE(VOID);	//FPS���v���A�X�V����֐�
VOID MY_FPS_WAIT(VOID);		//FPS�l���v�����A�҂֐�

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//�L�[�̓��͏�Ԃ��X�V����֐�

VOID MY_SOUND_LOAD(SOUND *, const char *);
VOID MY_IMAGE_LOAD(IMAGE *, int, int, const char *);

VOID SET_BLOCK_MOVE(IMAGE *, BLOCK);
VOID STAGE_FLAG_CHANGE_TRUE(BLOCK);
VOID INIT_STAGE_ALL(VOID);
VOID INIT_STAGE_ONLY_MOVE(VOID);
bool HOLD(VOID);
VOID DELETE_MOVE_LINE(VOID);
VOID REVERSE_BLOCK(VOID);
VOID RANKING_UPDATE(VOID);

VOID MY_GAME_TITLE(VOID);			//�^�C�g��
VOID MY_GAME_RANKING(VOID);			//�����L���O
VOID MY_GAME_SOZAI(VOID);			//�f�ޕ\��
VOID MY_GAME_PLAY_ENDLESS(VOID);	//�v���C�E�G���h���X
VOID MY_GAME_PLAY_TIME(VOID);		//�v���C�E�^�C���A�^�b�N
VOID MY_GAME_CHECK_ENDLESS(VOID);	//�m�F
VOID MY_GAME_CHECK_TIME(VOID);		//�m�F
VOID MY_GAME_END_CLEAR(VOID);		//�Q�[���N���A
VOID MY_GAME_END_OVER(VOID);		//�Q�[���I�[�o�[

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(GAME_WINDOW_MODECHANGE);	//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŉ�ʂ�\������

	SetWindowStyleMode(SET_WINDOW_ST_MODE_DEFAULT);	//�^�C�g���o�[

	SetMainWindowText(TEXT(GAME_WINDOW_NAME));		//�^�C�g���̕���

	if (DxLib_Init() == -1) { return -1; }	//DxLib����������

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

	ready_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B", 60, -1, DX_FONTTYPE_ANTIALIASING);
	Go_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B", 120, -1, DX_FONTTYPE_ANTIALIASING);
	score_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B", 39, -1, DX_FONTTYPE_ANTIALIASING);
	line_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B", 48, -1, DX_FONTTYPE_ANTIALIASING);
	rank_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B",32, -1, DX_FONTTYPE_ANTIALIASING);

	ChangeFont("UD �f�W�^�� ���ȏ��� NK-B");
	ChangeFontType(DX_FONTTYPE_ANTIALIASING);

	SetDrawScreen(DX_SCREEN_BACK);	//Draw�n�֐��͗���ʂɕ`��

	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//���b�Z�[�W�����̌��ʂ��G���[�̎��A�����I��

		if (ClearDrawScreen() != 0) { break; }	//��ʂ������o���Ȃ��������A�����I��

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

		MY_FPS_UPDATE();	//FPS�̏���(�X�V)

		ScreenFlip();		//���j�^�̃��t���b�V�����[�g�̑����ŗ���ʂ��ĕ`��

		MY_FPS_WAIT();		//FPS�̏���(�҂�)
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

//FPS���v���A�X�V����֐�
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0)
	{
		StartTimeFps = GetNowCount();	//����J�n�����ݒ�
	}

	if (CountFps == SampleNumFps)
	{
		int now = GetNowCount();		//���������擾
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)SampleNumFps);	//�v�Z
		CountFps = 0;	//�J�E���g���Z�b�g
		StartTimeFps = now;	//���������瑪��J�n
	}
	CountFps++;
	return;
}

//FPS�l���v�����A�҂֐�
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;	//������������
	int waitTime = CountFps * 1000 / GAME_FPS_SPEED - resultTime;	//�҂ׂ�����

	//�w���FPS�l���������ꍇ
	if (waitTime > 0)
	{
		Sleep(waitTime);	//�҂�
	}
	return;
}

//�L�[�̓��͏�Ԃ��X�V����֐�
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	char TempKey[256];	//�ꎞ�I�Ɍ��݂̃L�[�̓��͏�Ԃ��i�[����

	GetHitKeyStateAll(TempKey);	//�S�ẴL�[�̓��͏�Ԃ𓾂�

	for (int i = 0; i < 256; i++)	//������Ă���L�[�̃L�[�R�[�h�������Ă���Ƃ�
	{
		if (TempKey[i] != 0)	//������Ă���
		{
			AllKeyState[i]++;
		}
		else
		{
			AllKeyState[i] = 0;	//������Ă��Ȃ�
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

VOID SET_BLOCK_MOVE(IMAGE *i, BLOCK c)
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (stage_move[y][x] == c)
			{
				DrawGraph(x * 30 + 130, y * 30 + 75, i->handle, TRUE);
			}
		}
	}
	return;
}

VOID STAGE_FLAG_CHANGE_TRUE(BLOCK c)
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (stage_move[y][x] == c)
			{
				stage_put_flag[y][x] = true;
			}
		}
	}
	return;
}

VOID INIT_STAGE_ALL(VOID)
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			stage_move[y][x] = -1;
			stage_put_flag[y][x] = false;
		}
	}
	return;
}

VOID INIT_STAGE_ONLY_MOVE(VOID)
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (stage_move[y][x] != -1 && stage_put_flag[y][x] == false)
			{
				stage_move[y][x] = -1;
			}			
		}
	}
	return;
}

bool HOLD(VOID)
{	
	int hold_taihi = -1;			//HOLD�~�m����ւ������Ŏg�p
	if (holdfinish_flag == false)
	{
		if (firsthold_flag == true)
		{
			INIT_STAGE_ONLY_MOVE();
			holdmino = mino_rand;
			mino_rand = nextmino_rand;
			nextmino_rand = GetRand(MINO_KIND - 1);
			firsthold_flag = false;
			holdfinish_flag = true;

			return true;
		}
		else
		{
			INIT_STAGE_ONLY_MOVE();
			hold_taihi = holdmino;
			holdmino = mino_rand;
			mino_rand = hold_taihi;
			holdfinish_flag = true;

			return true;
		}
	}

	return false;
}

VOID REVERSE_BLOCK(VOID)
{
	int taihi_up;
	int taihi_down;

	for (int i = 0; i < 9; i++)
	{
		for (int x = 0; x < 10; x++)
		{
			taihi_up = stage_move[i][x];
			taihi_down = stage_move[17 - i][x];

			stage_move[i][x] = taihi_down;
			stage_move[17 - i][x] = taihi_up;

			if (stage_move[i][x] == -1)
			{
				stage_put_flag[i][x] = false;
			}
			else
			{
				stage_put_flag[i][x] = true;
			}

			if (stage_move[17 - i][x] == -1)
			{
				stage_put_flag[17 - i][x] = false;
			}
			else
			{
				stage_put_flag[17 - i][x] = true;
			}
		}
	}
	return;
}

VOID DELETE_MOVE_LINE(VOID)
{
	int deletecount = 0;	//����������

	int y_tmp = 0;	//��������̗�ԍ��̂P��̗�ԍ���ۑ�

	if (reverse_flag == false)
	{
		//��������
		for (int y = 0; y < 18; y++)
		{
			//�����S�Ă��u���b�N�Ŗ��܂�����
			if (stage_put_flag[y][0] == true &&
				stage_put_flag[y][1] == true &&
				stage_put_flag[y][2] == true &&
				stage_put_flag[y][3] == true &&
				stage_put_flag[y][4] == true &&
				stage_put_flag[y][5] == true &&
				stage_put_flag[y][6] == true &&
				stage_put_flag[y][7] == true &&
				stage_put_flag[y][8] == true &&
				stage_put_flag[y][9] == true)
			{
				//���܂�����̃u���b�N�����ׂď���
				for (int x = 0; x < 10; x++)
				{
					stage_move[y][x] = -1;
					stage_put_flag[y][x] = false;
				}

				y_tmp = y - 1;	//��ԍ���ۑ�

				//����������̏�̗�̃u���b�N�����ֈړ�
				for (int y = y_tmp; y > -1; y--)
				{
					for (int x = 0; x < 10; x++)
					{
						if (stage_move[y][x] != -1 && stage_put_flag[y][x] == true)
						{
							switch (stage_move[y][x])
							{
							case RED:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = RED;
								break;

							case ORANGE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = ORANGE;
								break;

							case YELLOW:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = YELLOW;
								break;

							case GREEN:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = GREEN;
								break;

							case RIGHTBLUE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = RIGHTBLUE;
								break;

							case BLUE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = BLUE;
								break;

							case PURPLE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y + 1][x] = true;
								stage_move[y + 1][x] = PURPLE;
								break;
							}
						}
					}
				}
				deletecount++;
			}
		}
	}
	else if (reverse_flag == true)
	{
		//��������
		for (int y = 17; y > -1; y--)
		{
			//�����S�Ă��u���b�N�Ŗ��܂�����
			if (stage_put_flag[y][0] == true &&
				stage_put_flag[y][1] == true &&
				stage_put_flag[y][2] == true &&
				stage_put_flag[y][3] == true &&
				stage_put_flag[y][4] == true &&
				stage_put_flag[y][5] == true &&
				stage_put_flag[y][6] == true &&
				stage_put_flag[y][7] == true &&
				stage_put_flag[y][8] == true &&
				stage_put_flag[y][9] == true)
			{
				//���܂�����̃u���b�N�����ׂď���
				for (int x = 0; x < 10; x++)
				{
					stage_move[y][x] = -1;
					stage_put_flag[y][x] = false;
				}

				y_tmp = y + 1;	//��ԍ���ۑ�
				
				//����������̉��̗�̃u���b�N����ֈړ�
				for (int y = y_tmp; y < 18; y++)
				{
					for (int x = 0; x < 10; x++)
					{
						if (stage_move[y][x] != -1 && stage_put_flag[y][x] == true)
						{
							switch (stage_move[y][x])
							{
							case RED:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = RED;
								break;

							case ORANGE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = ORANGE;
								break;

							case YELLOW:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = YELLOW;
								break;

							case GREEN:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = GREEN;
								break;

							case RIGHTBLUE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = RIGHTBLUE;
								break;

							case BLUE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = BLUE;
								break;

							case PURPLE:
								stage_move[y][x] = -1;
								stage_put_flag[y][x] = false;

								stage_put_flag[y - 1][x] = true;
								stage_move[y - 1][x] = PURPLE;
								break;
							}
						}
					}
				}
				deletecount++;
			}
		}
	}

	//�������C����&���]�܂ł̃��C�������f
	deleteline += deletecount;
	reverseline -= deletecount;

	if (reverseline <= 0)
	{
		reverseline += 40;
		if (reverse_flag == true)
		{
			reverse_flag = false;
		}
		else if (reverse_flag == false)
		{
			reverse_flag = true;
		}

		REVERSE_BLOCK();
	}

	//�X�R�A���Z
	switch (deletecount)
	{
	case 0:
		break;
	case 1: 
		score += 100;
		break;
	case 2:
		score += 300;
		break;
	case 3:
		score += 600;
		break;
	case 4:
		score += 1000;
		break;
	}

	return;
}

VOID RANKING_UPDATE(VOID)
{
	if (score_No1 < score)
	{
		score_No5 = score_No4;
		deleteline_No5 = deleteline_No4;

		score_No4 = score_No3;
		deleteline_No4 = deleteline_No3;

		score_No3 = score_No2;
		deleteline_No3 = deleteline_No2;

		score_No2 = score_No1;
		deleteline_No2 = deleteline_No1;

		score_No1 = score;
		deleteline_No1 = deleteline;
	}
	else if (score_No2 < score)
	{
		score_No5 = score_No4;
		deleteline_No5 = deleteline_No4;

		score_No4 = score_No3;
		deleteline_No4 = deleteline_No3;

		score_No3 = score_No2;
		deleteline_No3 = deleteline_No2;

		score_No2 = score;
		deleteline_No2 = deleteline;
	}
	else if (score_No3 < score)
	{
		score_No5 = score_No4;
		deleteline_No5 = deleteline_No4;

		score_No4 = score_No3;
		deleteline_No4 = deleteline_No3;

		score_No3 = score;
		deleteline_No3 = deleteline;
	}
	else if (score_No4 < score)
	{
		score_No5 = score_No4;
		deleteline_No5 = deleteline_No4;

		score_No4 = score;
		deleteline_No4 = deleteline;
	}
	else if (score_No5 < score)
	{
		score_No5 = score;
		deleteline_No5 = deleteline;
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

	INIT_STAGE_ALL();

	syoki_flag = 0;

	//ground_flag = false;		//�ڒn������
	firsthold_flag = true;		//�ŏ���HOLD���ǂ���
	holdfinish_flag = false;	//�ڒn����O��HOLD����x�ς�ł��邩

	first_flag = true;

	deleteline = 0;
	reverseline = 40;
	clearline = 200;
	score = 0;

	mino_rand = -1;
	nextmino_rand = -1;
	holdmino = -1;

	reverse_flag = false;

	play_flag = true;

	if (AllKeyState[KEY_INPUT_E] != 0)
	{
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

	/*�X�R�A�����L���O�\��*/
	DrawFormatStringToHandle(100, 146, GetColor(0, 0, 0), rank_fonthandle, "%8d", score_No1);
	DrawFormatStringToHandle(100, 184, GetColor(0, 0, 0), rank_fonthandle, "%8d", score_No2);
	DrawFormatStringToHandle(100, 222, GetColor(0, 0, 0), rank_fonthandle, "%8d", score_No3);
	DrawFormatStringToHandle(100, 260, GetColor(0, 0, 0), rank_fonthandle, "%8d", score_No4);
	DrawFormatStringToHandle(100, 298, GetColor(0, 0, 0), rank_fonthandle, "%8d", score_No5);
	/*�X�R�A�����L���O�\�������܂�*/

	/*�������C���\��*/
	DrawFormatStringToHandle(390, 146, GetColor(0, 0, 0), rank_fonthandle, "%3d", deleteline_No1);
	DrawFormatStringToHandle(390, 184, GetColor(0, 0, 0), rank_fonthandle, "%3d", deleteline_No2);
	DrawFormatStringToHandle(390, 222, GetColor(0, 0, 0), rank_fonthandle, "%3d", deleteline_No3);
	DrawFormatStringToHandle(390, 260, GetColor(0, 0, 0), rank_fonthandle, "%3d", deleteline_No4);
	DrawFormatStringToHandle(390, 298, GetColor(0, 0, 0), rank_fonthandle, "%3d", deleteline_No5);
	/*�������C���\�������܂�*/

	/*�^�C�������L���O�\��*/
	DrawFormatStringToHandle(190, 413, GetColor(0, 0, 0), rank_fonthandle, "%02d : %02d . %02d", No1.min, No1.sec, No1.m_sec);
	DrawFormatStringToHandle(190, 451, GetColor(0, 0, 0), rank_fonthandle, "%02d : %02d . %02d", No2.min, No2.sec, No2.m_sec);
	DrawFormatStringToHandle(190, 489, GetColor(0, 0, 0), rank_fonthandle, "%02d : %02d . %02d", No3.min, No3.sec, No3.m_sec);
	DrawFormatStringToHandle(190, 527, GetColor(0, 0, 0), rank_fonthandle, "%02d : %02d . %02d", No4.min, No4.sec, No4.m_sec);
	DrawFormatStringToHandle(190, 565, GetColor(0, 0, 0), rank_fonthandle, "%02d : %02d . %02d", No5.min, No5.sec, No5.m_sec);
	/*�^�C�������L���O�\�������܂�*/

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

	DrawString(0, 0, "�f�ޕ\�����", GetColor(255, 255, 255));
	DrawString(0, 30, "�^�C�g���ɖ߂�F�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	return;
}

VOID MY_GAME_PLAY_ENDLESS(VOID)
{
	/*��������*/
	DrawGraph(bg_play_endless.x, bg_play_endless.y, bg_play_endless.handle, TRUE);

	//���]�܂ł̎c�胉�C����
	DrawFormatStringToHandle(38, 320, GetColor(255, 100, 0), line_fonthandle, "%2d", reverseline);
	
	//�����������C��
	if (deleteline >= 999)	//�J���X�g
	{
		DrawFormatStringToHandle(444, 320, GetColor(0, 0, 255), line_fonthandle, "%3d", 999);
	}
	else
	{
		DrawFormatStringToHandle(444, 320, GetColor(0, 0, 255), line_fonthandle, "%3d", deleteline);
	}

	//�X�R�A
	if (score >= 99999999)	//�J���X�g
	{
		DrawFormatStringToHandle(210, 25, GetColor(255, 255, 255), score_fonthandle, "%08d", 99999999);
	}
	else
	{
		DrawFormatStringToHandle(210, 25, GetColor(255, 255, 255), score_fonthandle, "%08d", score);
	}

	static int syoki_temp;

	static int timer;	//�~�m�̗��� / �㏸�ɂĎg�p

	static int x_move;
	static int y_move;

	static int drop_tmp;
	static int rotation;

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

		SET_BLOCK_MOVE(&block_red, RED);
		SET_BLOCK_MOVE(&block_orange, ORANGE);
		SET_BLOCK_MOVE(&block_yellow, YELLOW);
		SET_BLOCK_MOVE(&block_green, GREEN);
		SET_BLOCK_MOVE(&block_rightblue, RIGHTBLUE);
		SET_BLOCK_MOVE(&block_blue, BLUE);
		SET_BLOCK_MOVE(&block_purple, PURPLE);
		
		timer = GetNowCount();

		//���삷��~�m
		switch (mino_rand)
		{
		case RED:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][5] == true || stage_put_flag[0][6] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						x_move = 5;
						if (stage_put_flag[0][4] == true || stage_put_flag[1][5] == true || stage_put_flag[1][6] == true)
						{
							y_move = -1;
						}
						else
						{
							y_move = 0;
						}
						rotation = 0;
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*��]����(Z�~�m�͉�]�̃p�^�[�����Q�ʂ�̂��ߍ��E�𕪂����ɏ���)*/
				if (AllKeyState[KEY_INPUT_Z] == 1 || AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[�̂Ƃ�
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ����
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						//90�x����Ă����ԂŉE�[�ŉ񂻂��Ƃ���ƍ��[�ɂ͂ݏo�邽�߁A1���ւ��炵�Ă����(���̎��ꕔ�̃u���b�N���ςɎc�邽�߂��������)
						if (x_move == 9)
						{
							//�ړ���ŉ񂹂邩�ǂ���
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move + 1][x_move] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
									stage_move[y_move][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if (stage_move[y_move + 1][x_move] == -1 && stage_move[y_move + 1][x_move + 1] == -1)	//�ʏ�E��[�E��ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
				}
				/*��]���������܂�*/

				if (rotation == 0)	//������
				{
					/*�u���b�N�\������*/
					if (y_move > -1)
					{
						stage_move[y_move][x_move - 1] = RED;
						stage_move[y_move][x_move] = RED;
					}
					stage_move[y_move + 1][x_move] = RED;
					stage_move[y_move + 1][x_move + 1] = RED;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 1][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true || stage_put_flag[y_move + 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RED);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move] = -1;
						stage_move[y_move + 1][x_move + 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							/*tmp += 1000;*/
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = RED;
							stage_move[y_move][x_move] = RED;
						}
						stage_move[y_move + 1][x_move] = RED;
						stage_move[y_move + 1][x_move + 1] = RED;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RED);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//����ȊO
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 2] == -1))	//����ȊO
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)	//�c����
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move] = RED;
					}
					if (y_move > -1)
					{
						stage_move[y_move][x_move] = RED;
						stage_move[y_move][x_move - 1] = RED;
					}
					stage_move[y_move + 1][x_move - 1] = RED;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 1][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RED);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false)
						{
							/*tmp += 1000;*/
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = RED;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = RED;
							stage_move[y_move][x_move - 1] = RED;
						}
						stage_move[y_move + 1][x_move - 1] = RED;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RED);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��[����
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move + 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//��[����
							(stage_move[y_move + 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][3] == true || stage_put_flag[17][4] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						x_move = 4;
						if (stage_put_flag[16][3] == true || stage_put_flag[16][4] == true || stage_put_flag[17][5] == true)
						{
							y_move = 18;
						}
						else
						{
							y_move = 17;
						}
						rotation = 0;
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*��]����(Z�~�m�͉�]�̃p�^�[�����Q�ʂ�̂��ߍ��E�𕪂����ɏ���)*/
				if (AllKeyState[KEY_INPUT_Z] == 1 || AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move][x_move - 1] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
						}
					}
					else if (rotation == 90)
					{
						//90�x����Ă����ԂŉE�[�ŉ񂻂��Ƃ���ƍ��[�ɂ͂ݏo�邽�߁A1���ւ��炵�Ă����(���̎��ꕔ�̃u���b�N���ςɎc�邽�߂��������)
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 2] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								if (y_move < 17)
								{
									stage_move[y_move + 1][x_move - 1] = -1;
								}
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move] = -1;
								if (y_move < 18)
								{
									stage_move[y_move][x_move] = -1;
									stage_move[y_move][x_move - 1] = -1;
								}
								if (y_move < 17)
								{
									stage_move[y_move + 1][x_move - 1] = -1;
								}
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							if (y_move < 17)
							{
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
					}
				}
				/*��]���������܂�*/

				if (rotation == 0)	//������
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = RED;
					stage_move[y_move - 1][x_move] = RED;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = RED;
						stage_move[y_move][x_move + 1] = RED;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true || stage_put_flag[y_move - 1][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RED);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						if (y_move < 18)
						{
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false)
						{
							/*tmp += 1000;*/
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�㏸����(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = RED;
						stage_move[y_move - 1][x_move] = RED;
						if (y_move < 18)
						{
							stage_move[y_move][x_move] = RED;
							stage_move[y_move][x_move + 1] = RED;
						}

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RED);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�㏸����(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)	//�c����
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move] = RED;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = RED;
						stage_move[y_move][x_move - 1] = RED;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move - 1] = RED;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 1][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RED);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move] = -1;
						if (y_move < 18)
						{
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
						}
						if (y_move < 17)
						{
							stage_move[y_move + 1][x_move - 1] = -1;
						}
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							/*tmp += 1000;*/
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							if (y_move < 17)
							{
								stage_move[y_move + 1][x_move - 1] = -1;
							}
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�㏸����(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							if (y_move < 17)
							{
								stage_move[y_move + 1][x_move - 1] = -1;
							}
							--y_move;
						}
						stage_move[y_move - 1][x_move] = RED;
						if (y_move < 18)
						{
							stage_move[y_move][x_move] = RED;
							stage_move[y_move][x_move - 1] = RED;
						}
						if (y_move < 17)
						{
							stage_move[y_move + 1][x_move - 1] = RED;
						}

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RED);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�㏸����(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���[����
							(stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							if (y_move < 17)
							{
								stage_move[y_move + 1][x_move - 1] = -1;
							}
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���[����
							(stage_move[y_move + 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							if (y_move < 18)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
							}
							if (y_move < 17)
							{
								stage_move[y_move + 1][x_move - 1] = -1;
							}
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}

			break;

		case ORANGE:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][4] == true || stage_put_flag[0][5] == true || stage_put_flag[0][6] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						if (stage_put_flag[1][4] == true || stage_put_flag[1][5] == true || stage_put_flag[1][6] == true)
						{
							y_move = -1;
						}
						else
						{
							y_move = 0;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*����]����*/
				if (AllKeyState[KEY_INPUT_Z] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move][x_move] == -1) ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							if (y_move > -1)
							{
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 180;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move + 1][x_move] == -1) ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))		//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*����]���������܂�*/

				/*�E��]����*/
				if (AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move][x_move - 1] == -1) ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 180;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move + 1][x_move] == -1) ||	//��[����
							(stage_move[y_move + 1][x_move] == -1 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1))		//�ʏ폈��
						{
							rotation = 270;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*�E��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move + 1] = ORANGE;
					}
					stage_move[y_move + 1][x_move - 1] = ORANGE;
					stage_move[y_move + 1][x_move] = ORANGE;
					stage_move[y_move + 1][x_move + 1] = ORANGE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true || stage_put_flag[y_move + 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move + 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						stage_move[y_move + 1][x_move + 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							/*tmp += 1000;*/
							if (y_move > -1)
							{
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move + 1] = ORANGE;
						}
						stage_move[y_move + 1][x_move - 1] = ORANGE;
						stage_move[y_move + 1][x_move] = ORANGE;
						stage_move[y_move + 1][x_move + 1] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 2] == -1 && stage_move[y_move + 1][x_move + 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move - 1] = ORANGE;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = ORANGE;
					}
					stage_move[y_move + 1][x_move - 1] = ORANGE;
					stage_move[y_move + 1][x_move] = ORANGE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;

						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							/*tmp += 1000;*/
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = ORANGE;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = ORANGE;
						}
						stage_move[y_move + 1][x_move - 1] = ORANGE;
						stage_move[y_move + 1][x_move] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 180)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = ORANGE;
						stage_move[y_move][x_move] = ORANGE;
						stage_move[y_move][x_move + 1] = ORANGE;
					}
					stage_move[y_move + 1][x_move - 1] = ORANGE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 1][x_move] == true || stage_put_flag[y_move + 1][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

						/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false && stage_put_flag[y_move + 1][x_move + 1] == false)
						{
							/*tmp += 1000;*/
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false && stage_put_flag[y_move + 1][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = ORANGE;
							stage_move[y_move][x_move] = ORANGE;
							stage_move[y_move][x_move + 1] = ORANGE;
						}
						stage_move[y_move + 1][x_move - 1] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 2] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 270)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move - 1] = ORANGE;
						stage_move[y_move - 1][x_move] = ORANGE;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = ORANGE;
					}
					stage_move[y_move + 1][x_move] = ORANGE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							/*tmp += 1000;*/
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = ORANGE;
							stage_move[y_move - 1][x_move] = ORANGE;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = ORANGE;
						}
						stage_move[y_move + 1][x_move] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{

						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][3] == true || stage_put_flag[17][4] == true || stage_put_flag[17][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						x_move = 4;
						if (stage_put_flag[16][3] == true || stage_put_flag[16][4] == true || stage_put_flag[16][5] == true)
						{
							y_move = 18;
						}
						else
						{
							y_move = 17;
						}
						rotation = 180;
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*����]����*/
				if (AllKeyState[KEY_INPUT_Z] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1) ||		//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1) ||		//���[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						//�E�[�ŉ񂷂Ƃ��ɍ��ɂ͂ݏo���Ȃ����߂̑΍��if��
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1))
						{
							rotation = 180;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							y_move == 17 ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))
						{
							rotation = 90;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						//�E�[�ŉ񂷂Ƃ��ɍ��ɂ͂ݏo���Ȃ����߂̑΍��if��
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move] == -1)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move] == -1 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*����]���������܂�*/

				/*�E��]����*/
				if (AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||		//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1) ||		//���[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						//���ɂ͂ݏo���Ȃ����߂̑΍��if��
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move] == -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (stage_move[y_move - 1][x_move] == -1 && stage_move[y_move - 1][x_move + 1] == -1)	//�ʏ�E���[�E���ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 180;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move][x_move] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						//���ɂ͂ݏo���Ȃ����߂̑΍��if��
						if (x_move == 9)
						{
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*�E��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move + 1] = ORANGE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = ORANGE;
						stage_move[y_move][x_move] = ORANGE;
						stage_move[y_move][x_move + 1] = ORANGE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 1][x_move - 1] == true || stage_put_flag[y_move - 1][x_move] == true || stage_put_flag[y_move - 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move + 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move][x_move + 1] = -1;
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							/*tmp += 1000;*/
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move + 1] = ORANGE;
						stage_move[y_move][x_move - 1] = ORANGE;
						stage_move[y_move][x_move] = ORANGE;
						stage_move[y_move][x_move + 1] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move - 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 2] == -1 && stage_move[y_move][x_move + 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = ORANGE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = ORANGE;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move - 1] = ORANGE;
						stage_move[y_move + 1][x_move] = ORANGE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move][x_move] == false)
						{
							/*tmp += 1000;*/
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = ORANGE;
						stage_move[y_move][x_move - 1] = ORANGE;
						stage_move[y_move + 1][x_move - 1] = ORANGE;
						stage_move[y_move + 1][x_move] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 180)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = ORANGE;
					stage_move[y_move - 1][x_move] = ORANGE;
					stage_move[y_move - 1][x_move + 1] = ORANGE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = ORANGE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true || stage_put_flag[y_move - 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move - 1][x_move + 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							/*tmp += 1000;*/
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = ORANGE;
						stage_move[y_move - 1][x_move] = ORANGE;
						stage_move[y_move - 1][x_move + 1] = ORANGE;
						stage_move[y_move][x_move - 1] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 2] == -1 && stage_move[y_move][x_move] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 270)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = ORANGE;
					stage_move[y_move - 1][x_move] = ORANGE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = ORANGE;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move] = ORANGE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(ORANGE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							/*tmp += 1000;*/
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = ORANGE;
						stage_move[y_move - 1][x_move] = ORANGE;
						stage_move[y_move][x_move] = ORANGE;
						stage_move[y_move + 1][x_move] = ORANGE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(ORANGE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}

			break;

		case YELLOW:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][4] == true || stage_put_flag[0][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						if (stage_put_flag[1][4] == true || stage_put_flag[1][5] == true)
						{
							y_move = -1;
						}
						else
						{
							y_move = 0;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*�u���b�N�\������*/
				if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
				{
					stage_move[y_move][x_move - 1] = YELLOW;
					stage_move[y_move][x_move] = YELLOW;
				}
				stage_move[y_move + 1][x_move - 1] = YELLOW;
				stage_move[y_move + 1][x_move] = YELLOW;
				/*�u���b�N�\�����������܂�*/

				/*�~������(���)*/
				if (timer > tmp)
				{
					/*�ڒn����*/
					if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
					{
						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(YELLOW);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�ڒn���������܂�*/
					tmp += 1000;
					if (y_move > -1)
					{
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
					}
					stage_move[y_move + 1][x_move - 1] = -1;
					stage_move[y_move + 1][x_move] = -1;
					++y_move;
				}
				/*�~������(���)�����܂�*/

				/*�~������(�\�t�g�h���b�v)*/
				if (timer > drop_tmp)
				{
					if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
					{
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					drop_tmp += 100;
				}
				/*�~������(�\�t�g�h���b�v�����܂�)*/

				/*�~������(�n�[�h�h���b�v)*/
				if (AllKeyState[KEY_INPUT_UP] == 1)
				{
					//������󔒂̊ԃ~�m������������
					while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
					{
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					if (y_move > -1)
					{
						stage_move[y_move][x_move - 1] = YELLOW;
						stage_move[y_move][x_move] = YELLOW;
					}
					stage_move[y_move + 1][x_move - 1] = YELLOW;
					stage_move[y_move + 1][x_move] = YELLOW;

					mino_rand = nextmino_rand;
					nextmino_rand = GetRand(MINO_KIND - 1);
					STAGE_FLAG_CHANGE_TRUE(YELLOW);
					holdfinish_flag = false;
					first_flag = true;
					/*tmp = 0;
					drop_tmp = 0;*/
					DELETE_MOVE_LINE();
					break;
				}
				/*�~������(�n�[�h�h���b�v)�����܂�*/

				/*���ړ�����*/
				if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
				{
					if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
						(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ�E��[����
					{
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						--x_move;
					}
				}
				/*���ړ����������܂�*/

				/*�E�ړ�����*/
				if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
				{
					if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
						(stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
					{
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++x_move;
					}
				}
				/*�E�ړ����������܂�*/
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][4] == true || stage_put_flag[17][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						if (stage_put_flag[16][4] == true || stage_put_flag[16][5] == true)
						{
							y_move = 18;
						}
						else
						{
							y_move = 17;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*�u���b�N�\������*/
				stage_move[y_move - 1][x_move - 1] = YELLOW;
				stage_move[y_move - 1][x_move] = YELLOW;
				if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
				{
					stage_move[y_move][x_move - 1] = YELLOW;
					stage_move[y_move][x_move] = YELLOW;
				}
				/*�u���b�N�\�����������܂�*/

				/*�㏸����(���)*/
				if (timer > tmp)
				{
					/*�ڒn����*/
					if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true)
					{
						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(YELLOW);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�ڒn���������܂�*/
					tmp += 1000;
					stage_move[y_move - 1][x_move - 1] = -1;
					stage_move[y_move - 1][x_move] = -1;
					stage_move[y_move][x_move - 1] = -1;
					stage_move[y_move][x_move] = -1;
					--y_move;
				}
				/*�㏸����(���)�����܂�*/

				/*�㏸����(�\�t�g�h���b�v)*/
				if (timer > drop_tmp)
				{
					if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
					{
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						--y_move;
					}
					drop_tmp += 100;
				}
				/*�㏸����(�\�t�g�h���b�v�����܂�)*/

				/*�~������(�n�[�h�h���b�v)*/
				if (AllKeyState[KEY_INPUT_UP] == 1)
				{
					//��オ�󔒂̊ԃ~�m���グ������
					while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
					{
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						--y_move;
					}
					stage_move[y_move - 1][x_move - 1] = YELLOW;
					stage_move[y_move - 1][x_move] = YELLOW;
					stage_move[y_move][x_move - 1] = YELLOW;
					stage_move[y_move][x_move] = YELLOW;

					mino_rand = nextmino_rand;
					nextmino_rand = GetRand(MINO_KIND - 1);
					STAGE_FLAG_CHANGE_TRUE(YELLOW);
					holdfinish_flag = false;
					first_flag = true;
					/*tmp = 0;
					drop_tmp = 0;*/
					DELETE_MOVE_LINE();
					break;
				}
				/*�~������(�n�[�h�h���b�v)�����܂�*/

				/*���ړ�����*/
				if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
				{
					if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
						(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1))	//�ʏ�E���[����
					{
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						--x_move;
					}
				}
				/*���ړ����������܂�*/

				/*�E�ړ�����*/
				if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
				{
					if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
						(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
					{
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						++x_move;
					}
				}
				/*�E�ړ����������܂�*/
			}

			break;

		case GREEN:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][4] == true || stage_put_flag[0][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						if (stage_put_flag[1][4] == true || stage_put_flag[1][5] == true || stage_put_flag[0][6] == true)
						{
							y_move = -1;
						}
						else
						{
							y_move = 0;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*��]����(S�~�m�͉�]�̃p�^�[�����Q�ʂ�̂��ߍ��E�𕪂����ɏ���)*/
				if (AllKeyState[KEY_INPUT_Z] == 1 || AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = GREEN;
						stage_move[y_move][x_move + 1] = GREEN;
					}
					stage_move[y_move + 1][x_move - 1] = GREEN;
					stage_move[y_move + 1][x_move] = GREEN;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 1][x_move + 1] == true || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(GREEN);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 1][x_move + 1] == false && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 1][x_move + 1] == false && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = GREEN;
							stage_move[y_move][x_move + 1] = GREEN;
						}
						stage_move[y_move + 1][x_move - 1] = GREEN;
						stage_move[y_move + 1][x_move] = GREEN;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(GREEN);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 2] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move - 1] = GREEN;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = GREEN;
						stage_move[y_move][x_move] = GREEN;
					}
					stage_move[y_move + 1][x_move] = GREEN;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 1][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(GREEN);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = GREEN;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = GREEN;
							stage_move[y_move][x_move] = GREEN;
						}
						stage_move[y_move + 1][x_move] = GREEN;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(GREEN);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][4] == true || stage_put_flag[17][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 4;
						if (stage_put_flag[17][3] == true || stage_put_flag[16][4] == true || stage_put_flag[16][5] == true)
						{
							y_move = 18;
						}
						else
						{
							y_move = 17;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*��]����(S�~�m�͉�]�̃p�^�[�����Q�ʂ�̂��ߍ��E�𕪂����ɏ���)*/
				if (AllKeyState[KEY_INPUT_Z] == 1 || AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (stage_move[y_move - 1][x_move] == -1 && stage_move[y_move - 1][x_move + 1] == -1)	//�ʏ�E���[�E���ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 0;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move] = GREEN;
					stage_move[y_move - 1][x_move + 1] = GREEN;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = GREEN;
						stage_move[y_move][x_move] = GREEN;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 1][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true || stage_put_flag[y_move - 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(GREEN);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move - 1][x_move + 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move] = GREEN;
						stage_move[y_move - 1][x_move + 1] = GREEN;
						stage_move[y_move][x_move - 1] = GREEN;
						stage_move[y_move][x_move] = GREEN;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(GREEN);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 1] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 1] == -1 && stage_move[y_move - 1][x_move + 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = GREEN;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = GREEN;
						stage_move[y_move][x_move] = GREEN;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move] = GREEN;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 1][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(GREEN);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move] = -1;
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = GREEN;
						stage_move[y_move][x_move - 1] = GREEN;
						stage_move[y_move][x_move] = GREEN;
						stage_move[y_move + 1][x_move] = GREEN;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(GREEN);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}

			break;

		case RIGHTBLUE:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][3] == true || stage_put_flag[0][4] == true || stage_put_flag[0][5] == true || stage_put_flag[0][6] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						y_move = 0;
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*��]����(I�~�m�͉�]�̃p�^�[�����Q�ʂ�̂��ߍ��E�𕪂����ɏ���)*/
				if (AllKeyState[KEY_INPUT_Z] == 1 || AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == 17)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move - 3][x_move] == -1 && stage_move[y_move - 2][x_move] == -1 && stage_move[y_move - 1][x_move] == -1)
							{
								stage_move[y_move][x_move - 2] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move + 1] = -1;
								--y_move;
								rotation = 90;
								stage_move[y_move][x_move - 2] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
						}
						else if ((y_move == 0 && stage_move[y_move + 1][x_move] == -1) ||	//��[�ŉ񂷏ꍇ
							(y_move == 1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move] == -1) ||	//�ォ��Q�Ԗڂŉ񂷏ꍇ
							(stage_move[y_move - 2][x_move] == -1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move] == -1))		//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move][x_move - 2] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 2][x_move - 1] == -1 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move][x_move - 3] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1)
							{
								if (y_move > 1)
								{
									stage_move[y_move - 2][x_move] = -1;
								}
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								if (y_move > 1)
								{
									stage_move[y_move - 2][x_move] = -1;
								}
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (x_move == 0)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 2][x_move + 2] == -1 && stage_move[y_move - 1][x_move + 2] == -1 && stage_move[y_move + 1][x_move + 2] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1 && stage_move[y_move][x_move + 3] == -1)
							{
								if (y_move > 1)
								{
									stage_move[y_move - 2][x_move] = -1;
								}
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								x_move += 2;
								rotation = 0;
								if (y_move > 1)
								{
									stage_move[y_move - 2][x_move] = -1;
								}
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (x_move == 1)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 2][x_move + 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1)
							{
								if (y_move > 1)
								{
									stage_move[y_move - 2][x_move] = -1;
								}
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								++x_move;
								rotation = 0;
								if (y_move > 1)
								{
									stage_move[y_move - 2][x_move] = -1;
								}
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (stage_move[y_move][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1)	//�ʏ�E��[�E��ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							//���[�ŉ���Ԃ���c�ɉ񂵂������ɖ߂��ƂȂ����ςȂƂ��Ƀu���b�N���c�邽�߂������������
							if (y_move == 17)
							{
								stage_move[y_move - 3][x_move] = -1;
							}
							rotation = 0;
							if (y_move > 1)
							{
								stage_move[y_move - 2][x_move] = -1;
							}
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					stage_move[y_move][x_move - 2] = RIGHTBLUE;
					stage_move[y_move][x_move - 1] = RIGHTBLUE;
					stage_move[y_move][x_move] = RIGHTBLUE;
					stage_move[y_move][x_move + 1] = RIGHTBLUE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 17 || stage_put_flag[y_move + 1][x_move - 2] == true || stage_put_flag[y_move + 1][x_move - 1] == true || stage_put_flag[y_move + 1][x_move] == true || stage_put_flag[y_move + 1][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move][x_move - 2] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move][x_move + 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 17 && stage_put_flag[y_move + 1][x_move - 2] == false && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false && stage_put_flag[y_move + 1][x_move + 1] == false)
						{
							stage_move[y_move][x_move - 2] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 17 && stage_put_flag[y_move + 1][x_move - 2] == false && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false && stage_put_flag[y_move + 1][x_move + 1] == false)
						{
							stage_move[y_move][x_move - 2] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							++y_move;
						}
						stage_move[y_move][x_move - 2] = RIGHTBLUE;
						stage_move[y_move][x_move - 1] = RIGHTBLUE;
						stage_move[y_move][x_move] = RIGHTBLUE;
						stage_move[y_move][x_move + 1] = RIGHTBLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 3)
					{
						if (stage_move[y_move][x_move - 3] == -1)	//�ʏ�E��[����(��ɂ͂ݏo��p�^�[���͖���)
						{
							stage_move[y_move][x_move - 2] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if (stage_move[y_move][x_move + 2] == -1)	//�ʏ�E��[����(��ɂ͂ݏo��p�^�[���͖���)
						{
							stage_move[y_move][x_move - 2] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					if (y_move > 1)	//�ォ��Q�Ԗڂ̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 2][x_move] = RIGHTBLUE;
					}
					if (y_move > 0)	//��[�̏ꍇ�͏�Q�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move] = RIGHTBLUE;
					}
					stage_move[y_move][x_move] = RIGHTBLUE;
					stage_move[y_move + 1][x_move] = RIGHTBLUE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 1)
						{
							stage_move[y_move - 2][x_move] = -1;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = -1;
						}
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 1)
							{
								stage_move[y_move - 2][x_move] = -1;
							}
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 1)
							{
								stage_move[y_move - 2][x_move] = -1;
							}
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > 1)
						{
							stage_move[y_move - 2][x_move] = RIGHTBLUE;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = RIGHTBLUE;
						}
						stage_move[y_move][x_move] = RIGHTBLUE;
						stage_move[y_move + 1][x_move] = RIGHTBLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 1)
					{
						if ((y_move == 0 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[����
							(y_move == 1 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//�ォ��Q�Ԗڂ̂Ƃ�
							(stage_move[y_move - 2][x_move - 1] == -1 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							if (y_move > 1)
							{
								stage_move[y_move - 2][x_move] = -1;
							}
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 0 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��[����
							(y_move == 1 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//�ォ��Q�Ԗڂ̂Ƃ�
							(stage_move[y_move - 2][x_move + 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 1)
							{
								stage_move[y_move - 2][x_move] = -1;
							}
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][3] == true || stage_put_flag[17][4] == true || stage_put_flag[17][5] == true || stage_put_flag[17][6] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 4;
						y_move = 17;
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*��]����(I�~�m�͉�]�̃p�^�[�����Q�ʂ�̂��ߍ��E�𕪂����ɏ���)*/
				if (AllKeyState[KEY_INPUT_Z] == 1 || AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						//��[�ŉ񂷏ꍇ
						if (y_move == 0)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 2] == -1 && stage_move[y_move + 3][x_move] == -1 && stage_move[y_move + 2][x_move] == -1 && stage_move[y_move + 1][x_move] == -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move + 1] = -1;
								stage_move[y_move][x_move + 2] = -1;
								y_move++;
								rotation = 90;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
								stage_move[y_move][x_move + 2] = -1;
							}
						}
						else if ((y_move == 17 && stage_move[y_move - 1][x_move] == -1) ||	//���[�ŉ񂷏ꍇ
							(y_move == 16 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move] == -1) ||	//������Q�Ԗڂŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move] == -1 && stage_move[y_move + 2][x_move] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							stage_move[y_move][x_move + 2] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move + 2][x_move - 2] == -1 && stage_move[y_move][x_move - 3] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								stage_move[y_move + 2][x_move] = -1;
								x_move -= 2;
								rotation = 0;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								stage_move[y_move + 2][x_move] = -1;
							}
						}
						else if (x_move == 8)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 2][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								stage_move[y_move + 2][x_move] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								stage_move[y_move + 2][x_move] = -1;
							}
						}
						else if (x_move == 0)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1 && stage_move[y_move + 2][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1 && stage_move[y_move][x_move + 3] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								stage_move[y_move + 2][x_move] = -1;
								++x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								stage_move[y_move + 2][x_move] = -1;
							}
						}
						else if (stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1)	//�ʏ�E���[����(���ɂ͂ݏo��p�^�[���͖���)
						{
							//��[�ŉ���Ԃ���c�ɉ񂵂������ɖ߂��ƂȂ����ςȂƂ��Ƀu���b�N���c�邽�߂������������
							if (y_move == 0)
							{
								stage_move[y_move + 3][x_move] = -1;
							}
							rotation = 0;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 2][x_move] = -1;
						}
					}
				}
				/*��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					stage_move[y_move][x_move - 1] = RIGHTBLUE;
					stage_move[y_move][x_move] = RIGHTBLUE;
					stage_move[y_move][x_move + 1] = RIGHTBLUE;
					stage_move[y_move][x_move + 2] = RIGHTBLUE;
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 0 || stage_put_flag[y_move - 1][x_move - 1] == true || stage_put_flag[y_move - 1][x_move] == true || stage_put_flag[y_move - 1][x_move + 1] == true || stage_put_flag[y_move - 1][x_move + 2] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move][x_move + 1] = -1;
						stage_move[y_move][x_move + 2] = -1;
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 0 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false && stage_put_flag[y_move - 1][x_move + 2] == false)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							stage_move[y_move][x_move + 2] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 0 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false && stage_put_flag[y_move - 1][x_move + 2] == false)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							stage_move[y_move][x_move + 2] = -1;
							--y_move;
						}
						stage_move[y_move][x_move - 1] = RIGHTBLUE;
						stage_move[y_move][x_move] = RIGHTBLUE;
						stage_move[y_move][x_move + 1] = RIGHTBLUE;
						stage_move[y_move][x_move + 2] = RIGHTBLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if (stage_move[y_move][x_move - 2] == -1)	//�ʏ�E���[����(���ɂ͂ݏo��p�^�[���͖���)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							stage_move[y_move][x_move + 2] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 6)
					{
						if (stage_move[y_move][x_move + 3] == -1)	//�ʏ�E���[����(���ɂ͂ݏo��p�^�[���͖���)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							stage_move[y_move][x_move + 2] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move] = RIGHTBLUE;
					stage_move[y_move][x_move] = RIGHTBLUE;
					if (y_move < 17)
					{
						stage_move[y_move + 1][x_move] = RIGHTBLUE;
					}
					if (y_move < 16)
					{
						stage_move[y_move + 2][x_move] = RIGHTBLUE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move] = -1;
						stage_move[y_move + 2][x_move] = -1;
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 2][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 2][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move] = RIGHTBLUE;
						stage_move[y_move][x_move] = RIGHTBLUE;
						stage_move[y_move + 1][x_move] = RIGHTBLUE;
						stage_move[y_move + 2][x_move] = RIGHTBLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(RIGHTBLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 1)
					{
						if ((y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1) ||	//���[����
							(y_move == 16 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//������Q�Ԗڂ̂Ƃ�
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 2][x_move - 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 2][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 17 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//���[����
							(y_move == 16 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//������Q�Ԗڂ̂Ƃ�
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1 && stage_move[y_move + 2][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 2][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}

			break;

		case BLUE:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][4] == true || stage_put_flag[0][5] == true || stage_put_flag[0][6] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						if (stage_put_flag[1][4] == true || stage_put_flag[1][5] == true || stage_put_flag[1][6] == true)
						{
							y_move = -1;
						}
						else
						{
							y_move = 0;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*����]����*/
				if (AllKeyState[KEY_INPUT_Z] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move][x_move] == -1) ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								--x_move;
								rotation = 180;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 180;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move - 1] == -1))		//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move + 1][x_move] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if (stage_move[y_move + 1][x_move] == -1 && stage_move[y_move + 1][x_move + 1] == -1)	//�ʏ�E��[�E��ɂ͂ݏo���Ƃ��A���ׂď����͓���
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
				}
				/*����]���������܂�*/

				/*�E��]����*/
				if (AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							y_move == 0 ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
								--x_move;
								rotation = 180;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 180;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
					else if (rotation == 180)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*�E��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = BLUE;
					}
					stage_move[y_move + 1][x_move - 1] = BLUE;
					stage_move[y_move + 1][x_move] = BLUE;
					stage_move[y_move + 1][x_move + 1] = BLUE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true || stage_put_flag[y_move + 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						stage_move[y_move + 1][x_move + 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = BLUE;
						}
						stage_move[y_move + 1][x_move - 1] = BLUE;
						stage_move[y_move + 1][x_move] = BLUE;
						stage_move[y_move + 1][x_move + 1] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move + 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move - 1] = BLUE;
						stage_move[y_move - 1][x_move] = BLUE;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = BLUE;
					}
					stage_move[y_move + 1][x_move - 1] = BLUE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = BLUE;
							stage_move[y_move - 1][x_move] = BLUE;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = BLUE;
						}
						stage_move[y_move + 1][x_move - 1] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 180)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = BLUE;
						stage_move[y_move][x_move] = BLUE;
						stage_move[y_move][x_move + 1] = BLUE;
					}
					stage_move[y_move + 1][x_move + 1] = BLUE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 1][x_move - 1] == true || stage_put_flag[y_move + 1][x_move] == true || stage_put_flag[y_move + 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
						stage_move[y_move + 1][x_move + 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = BLUE;
							stage_move[y_move][x_move] = BLUE;
							stage_move[y_move][x_move + 1] = BLUE;
						}
						stage_move[y_move + 1][x_move + 1] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 2] == -1 && stage_move[y_move + 1][x_move + 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 270)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move] = BLUE;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = BLUE;
					}
					stage_move[y_move + 1][x_move - 1] = BLUE;
					stage_move[y_move + 1][x_move] = BLUE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = BLUE;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = BLUE;
						}
						stage_move[y_move + 1][x_move - 1] = BLUE;
						stage_move[y_move + 1][x_move] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][3] == true || stage_put_flag[17][4] == true || stage_put_flag[17][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 180;
						x_move = 4;
						if (stage_put_flag[16][3] == true || stage_put_flag[16][4] == true || stage_put_flag[16][5] == true)
						{
							y_move = 18;
						}
						else
						{
							y_move = 17;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*����]����*/
				if (AllKeyState[KEY_INPUT_Z] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move - 1][x_move] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 2] == -1)
							{
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 180;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move][x_move - 1] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
				}
				/*����]���������܂�*/

				/*�E��]����*/
				if (AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move - 1][x_move] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								--x_move;
								rotation = 180;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 180;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move][x_move] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*�E��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = BLUE;
					if (y_move < 18)	//���ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = BLUE;
						stage_move[y_move][x_move] = BLUE;
						stage_move[y_move][x_move + 1] = BLUE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�㏸����(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 1][x_move] == true || stage_put_flag[y_move - 1][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							tmp = 0;
							drop_tmp = 0;
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move][x_move + 1] = -1;
						--y_move;
					}
					/*�㏸����(���)�����܂�*/

					/*�㏸����(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�㏸����(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = BLUE;
						stage_move[y_move][x_move - 1] = BLUE;
						stage_move[y_move][x_move] = BLUE;
						stage_move[y_move][x_move + 1] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 2] == -1)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = BLUE;
					stage_move[y_move - 1][x_move] = BLUE;
					if (y_move < 18)	//���ɂ͂ݏo�����͉�������\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = BLUE;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move - 1] = BLUE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							tmp = 0;
							drop_tmp = 0;
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move + 1][x_move - 1] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = BLUE;
						stage_move[y_move - 1][x_move] = BLUE;
						stage_move[y_move][x_move - 1] = BLUE;
						stage_move[y_move + 1][x_move - 1] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 180)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = BLUE;
					stage_move[y_move - 1][x_move] = BLUE;
					stage_move[y_move - 1][x_move + 1] = BLUE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move + 1] = BLUE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true || stage_put_flag[y_move - 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							tmp = 0;
							drop_tmp = 0;
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move - 1][x_move + 1] = -1;
						stage_move[y_move][x_move + 1] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = BLUE;
						stage_move[y_move - 1][x_move] = BLUE;
						stage_move[y_move - 1][x_move + 1] = BLUE;
						stage_move[y_move][x_move + 1] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7 && stage_move[y_move - 1][x_move + 2] == -1 && stage_move[y_move][x_move + 2] == -1)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 2] == -1 && stage_move[y_move][x_move + 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move + 1] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 270)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move] = BLUE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = BLUE;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move - 1] = BLUE;
						stage_move[y_move + 1][x_move] = BLUE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(BLUE);
							holdfinish_flag = false;
							first_flag = true;
							tmp = 0;
							drop_tmp = 0;
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move] = BLUE;
						stage_move[y_move][x_move] = BLUE;
						stage_move[y_move + 1][x_move - 1] = BLUE;
						stage_move[y_move + 1][x_move] = BLUE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(BLUE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--x_move;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							++x_move;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}

			break;

		case PURPLE:
			if (reverse_flag == false)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[0][4] == true || stage_put_flag[0][5] == true || stage_put_flag[0][6] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 0;
						x_move = 5;
						if (stage_put_flag[1][4] == true || stage_put_flag[1][5] == true || stage_put_flag[1][6] == true)
						{
							y_move = -1;
						}
						else
						{
							y_move = 0;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*����]����*/
				if (AllKeyState[KEY_INPUT_Z] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move - 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							if (y_move < -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
							}

						}
						else if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move + 1] == -1))	//�ʏ�E��[����
						{
							rotation = 180;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move + 1][x_move] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if (stage_move[y_move + 1][x_move] == -1 && stage_move[y_move + 1][x_move + 1] == -1)	//�ʏ�E��[�E��ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
				}
				/*����]���������܂�*/

				/*�E��]����*/
				if (AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 0 && stage_move[y_move][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								--x_move;
								rotation = 180;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move - 1] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ�E��[����
						{
							rotation = 180;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == -1 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							y_move == 0 ||	//��[����
							stage_move[y_move - 1][x_move] == -1)	//�ʏ폈��
						{
							rotation = 270;
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1)
							{
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move] = -1;
								}
								--x_move;
								rotation = 0;
								if (y_move > 0)
								{
									stage_move[y_move - 1][x_move] = -1;
								}
								if (y_move > -1)
								{
									stage_move[y_move][x_move - 1] = -1;
									stage_move[y_move][x_move] = -1;
								}
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1)	//�ʏ�E��[�E��ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 0;
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*�E��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = PURPLE;
					}
					stage_move[y_move + 1][x_move - 1] = PURPLE;
					stage_move[y_move + 1][x_move] = PURPLE;
					stage_move[y_move + 1][x_move + 1] = PURPLE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true || stage_put_flag[y_move + 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						stage_move[y_move + 1][x_move] = -1;
						stage_move[y_move + 1][x_move + 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 2][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move] = PURPLE;
						}
						stage_move[y_move + 1][x_move - 1] = PURPLE;
						stage_move[y_move + 1][x_move] = PURPLE;
						stage_move[y_move + 1][x_move + 1] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 2] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							stage_move[y_move + 1][x_move] = -1;
							stage_move[y_move + 1][x_move + 1] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move - 1] = PURPLE;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
					}
					stage_move[y_move + 1][x_move - 1] = PURPLE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 2][x_move - 1] == true || stage_put_flag[y_move + 1][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move - 1] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 2][x_move - 1] == false && stage_put_flag[y_move + 1][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move - 1] = PURPLE;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = PURPLE;
							stage_move[y_move][x_move] = PURPLE;
						}
						stage_move[y_move + 1][x_move - 1] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move - 1] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move - 1] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 180)
				{
					/*�u���b�N�\������*/
					if (y_move > -1)
					{
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
						stage_move[y_move][x_move + 1] = PURPLE;
					}
					stage_move[y_move + 1][x_move] = PURPLE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 1][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true || stage_put_flag[y_move + 1][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 1][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false && stage_put_flag[y_move + 1][x_move + 1] == false)
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = PURPLE;
							stage_move[y_move][x_move] = PURPLE;
							stage_move[y_move][x_move + 1] = PURPLE;
						}
						stage_move[y_move + 1][x_move] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(stage_move[y_move][x_move + 2] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ�E��[����
						{
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move][x_move + 1] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 270)
				{
					/*�u���b�N�\������*/
					if (y_move > 0)	//��[�̏ꍇ�͏�P�񂾂��\�����Ȃ�
					{
						stage_move[y_move - 1][x_move] = PURPLE;
					}
					if (y_move > -1)	//��ɂ͂ݏo����㕔�͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
					}
					stage_move[y_move + 1][x_move] = PURPLE;
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 16 || stage_put_flag[y_move + 1][x_move - 1] == true || stage_put_flag[y_move + 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = -1;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
						stage_move[y_move + 1][x_move] = -1;
						++y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//������󔒂̊ԃ~�m������������
						while (y_move < 16 && stage_put_flag[y_move + 1][x_move - 1] == false && stage_put_flag[y_move + 2][x_move] == false)
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							++y_move;
						}
						if (y_move > 0)
						{
							stage_move[y_move - 1][x_move] = PURPLE;
						}
						if (y_move > -1)
						{
							stage_move[y_move][x_move - 1] = PURPLE;
							stage_move[y_move][x_move] = PURPLE;
						}
						stage_move[y_move + 1][x_move] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��ɂ͂ݏo���Ƃ�
							(y_move == 0 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1) ||	//��[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							if (y_move > 0)
							{
								stage_move[y_move - 1][x_move] = -1;
							}
							if (y_move > -1)
							{
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
							}
							stage_move[y_move + 1][x_move] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}
			else if (reverse_flag == true)
			{
				/*����������*/
				if (first_flag == true)
				{
					if (stage_put_flag[17][3] == true || stage_put_flag[17][4] == true || stage_put_flag[17][5] == true)
					{
						/*�����L���O�X�V����*/
						RANKING_UPDATE();
						/*�����L���O�X�V���������܂�*/

						//StopSoundMem(bgm_play.handle);
						GameSceneNow = (int)GAME_SCENE_END_OVER;
						return;
					}
					else
					{
						tmp = timer + 1000;
						drop_tmp = timer + 100;
						rotation = 180;
						x_move = 4;
						if (stage_put_flag[16][3] == true || stage_put_flag[16][4] == true || stage_put_flag[16][5] == true)
						{
							y_move = 18;
						}
						else
						{
							y_move = 17;
						}
						first_flag = false;
					}
				}
				/*���������������܂�*/

				/*HOLD����*/
				if (AllKeyState[KEY_INPUT_SPACE] == 1)
				{
					if (HOLD())
					{
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						break;
					}
				}
				/*HOLD���������܂�*/

				/*����]����*/
				if (AllKeyState[KEY_INPUT_Z] == 1)
				{
					if (rotation == 0)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							y_move == 17 ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move - 2] == -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 180;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move - 1][x_move + 1] == -1)	//�ʏ�E���[�E���ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 180;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == 18 ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move][x_move - 1] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1)
							{
								stage_move[y_move + 1][x_move - 1] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
				}
				/*����]���������܂�*/

				/*�E��]����*/
				if (AllKeyState[KEY_INPUT_C] == 1)
				{
					if (rotation == 0)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							rotation = 90;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
						}
					}
					else if (rotation == 90)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1 && stage_move[y_move - 1][x_move] == -1)
							{
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
								--x_move;
								rotation = 180;
								stage_move[y_move - 1][x_move - 1] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move - 1] = -1;
							}
						}
						else if (stage_move[y_move - 1][x_move] == -1 && stage_move[y_move - 1][x_move + 1] == -1)	//�ʏ�E���[�E���ɂ͂ݏo���Ƃ��A�S�ď����͓���
						{
							rotation = 180;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
						}
					}
					else if (rotation == 180)
					{
						if (y_move == 18 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(y_move == 17 && stage_move[y_move][x_move - 1] == -1) ||	//���[�ŉ񂷏ꍇ
							(stage_move[y_move][x_move - 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							rotation = 270;
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move] = -1;
						}
					}
					else if (rotation == 270)
					{
						if (x_move == 9)
						{
							//�ړ���Ƀu���b�N���������
							if (stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1)
							{
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
								--x_move;
								rotation = 0;
								stage_move[y_move - 1][x_move] = -1;
								stage_move[y_move][x_move - 1] = -1;
								stage_move[y_move][x_move] = -1;
								stage_move[y_move + 1][x_move] = -1;
							}
						}
						else if (y_move == 18 ||	//��ɂ͂ݏo���Ƃ��ɉ񂷏ꍇ
							(stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							rotation = 0;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
						}
					}
				}
				/*�E��]���������܂�*/

				if (rotation == 0)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move] = PURPLE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
						stage_move[y_move][x_move + 1] = PURPLE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 1][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true || stage_put_flag[y_move - 1][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move][x_move + 1] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 1][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move] = PURPLE;
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
						stage_move[y_move][x_move + 1] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 2] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move][x_move + 1] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 90)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = PURPLE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move - 1] = PURPLE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 1][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move - 1] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 1][x_move] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = PURPLE;
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
						stage_move[y_move + 1][x_move - 1] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 2] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move - 1] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 180)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move - 1] = PURPLE;
					stage_move[y_move - 1][x_move] = PURPLE;
					stage_move[y_move - 1][x_move + 1] = PURPLE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move] = PURPLE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 2][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true || stage_put_flag[y_move - 2][x_move + 1] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move - 1] = -1;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move - 1][x_move + 1] = -1;
						stage_move[y_move][x_move] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 2][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false && stage_put_flag[y_move - 2][x_move + 1] == false)
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move - 1] = PURPLE;
						stage_move[y_move - 1][x_move] = PURPLE;
						stage_move[y_move - 1][x_move + 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move - 2] == -1 && stage_move[y_move][x_move - 1] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 7)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 2] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(stage_move[y_move - 1][x_move + 2] == -1 && stage_move[y_move][x_move + 1] == -1))	//�ʏ�E���[����
						{
							stage_move[y_move - 1][x_move - 1] = -1;
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move - 1][x_move + 1] = -1;
							stage_move[y_move][x_move] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
				else if (rotation == 270)
				{
					/*�u���b�N�\������*/
					stage_move[y_move - 1][x_move] = PURPLE;
					if (y_move < 18)	//���ɂ͂ݏo���牺�����͕\�����Ȃ�
					{
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
					}
					if (y_move < 17)	//���[���͉�������\�����Ȃ�
					{
						stage_move[y_move + 1][x_move] = PURPLE;
					}
					/*�u���b�N�\�����������܂�*/

					/*�~������(���)*/
					if (timer > tmp)
					{
						/*�ڒn����*/
						if (y_move == 1 || stage_put_flag[y_move - 1][x_move - 1] == true || stage_put_flag[y_move - 2][x_move] == true)
						{
							mino_rand = nextmino_rand;
							nextmino_rand = GetRand(MINO_KIND - 1);
							STAGE_FLAG_CHANGE_TRUE(PURPLE);
							holdfinish_flag = false;
							first_flag = true;
							/*tmp = 0;
							drop_tmp = 0;*/
							DELETE_MOVE_LINE();
							break;
						}
						/*�ڒn���������܂�*/
						tmp += 1000;
						stage_move[y_move - 1][x_move] = -1;
						stage_move[y_move][x_move - 1] = -1;
						stage_move[y_move][x_move] = -1;
						stage_move[y_move + 1][x_move] = -1;
						--y_move;
					}
					/*�~������(���)�����܂�*/

					/*�~������(�\�t�g�h���b�v)*/
					if (timer > drop_tmp)
					{
						if (AllKeyState[KEY_INPUT_DOWN] != 0 && y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						drop_tmp += 100;
					}
					/*�~������(�\�t�g�h���b�v�����܂�)*/

					/*�~������(�n�[�h�h���b�v)*/
					if (AllKeyState[KEY_INPUT_UP] == 1)
					{
						//��オ�󔒂̊ԃ~�m���グ������
						while (y_move > 1 && stage_put_flag[y_move - 1][x_move - 1] == false && stage_put_flag[y_move - 2][x_move] == false)
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							--y_move;
						}
						stage_move[y_move - 1][x_move] = PURPLE;
						stage_move[y_move][x_move - 1] = PURPLE;
						stage_move[y_move][x_move] = PURPLE;
						stage_move[y_move + 1][x_move] = PURPLE;

						mino_rand = nextmino_rand;
						nextmino_rand = GetRand(MINO_KIND - 1);
						STAGE_FLAG_CHANGE_TRUE(PURPLE);
						holdfinish_flag = false;
						first_flag = true;
						/*tmp = 0;
						drop_tmp = 0;*/
						DELETE_MOVE_LINE();
						break;
					}
					/*�~������(�n�[�h�h���b�v)�����܂�*/

					/*���ړ�����*/
					if (AllKeyState[KEY_INPUT_LEFT] == 1 && x_move >= 2)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move - 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move - 1] == -1 && stage_move[y_move][x_move - 2] == -1 && stage_move[y_move + 1][x_move - 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							x_move--;
						}
					}
					/*���ړ����������܂�*/

					/*�E�ړ�����*/
					if (AllKeyState[KEY_INPUT_RIGHT] == 1 && x_move <= 8)
					{
						if ((y_move == 18 && stage_move[y_move - 1][x_move + 1] == -1) ||	//���ɂ͂ݏo���Ƃ�
							(y_move == 17 && stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1) ||	//���[����
							(stage_move[y_move - 1][x_move + 1] == -1 && stage_move[y_move][x_move + 1] == -1 && stage_move[y_move + 1][x_move + 1] == -1))	//�ʏ폈��
						{
							stage_move[y_move - 1][x_move] = -1;
							stage_move[y_move][x_move - 1] = -1;
							stage_move[y_move][x_move] = -1;
							stage_move[y_move + 1][x_move] = -1;
							x_move++;
						}
					}
					/*�E�ړ����������܂�*/
				}
			}

			break;
		}

		//���ɑ��삷��~�m
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

		//HOLD���Ă����~�m
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

	if (AllKeyState[KEY_INPUT_BACK] != 0)
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
	static int stop_timer;	//�~�܂��Ă��鎞�Ԃ��v��
	stop_timer = GetNowCount();

	SetFontSize(24);
	DrawString(0, 0, "�m�F���(�G���h���X)", GetColor(255, 255, 255));
	DrawString(0, 30, "�Q�[�����I�����܂����H", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C�g���F�x�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�v���C�𑱍s�F�m�L�[", GetColor(255, 255, 255));
	
	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		//StopSoundMem(bgm_play.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_N] != 0)
	{
		tmp = stop_timer + 1000;	//�P�b��������tmp�ɑ��
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}

	return;
}

VOID MY_GAME_CHECK_TIME(VOID)
{
	SetFontSize(24);
	DrawString(0, 0, "�m�F���(�G���h���X)", GetColor(255, 255, 255));
	DrawString(0, 30, "�Q�[�����I�����܂����H", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C�g���F�x�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�v���C�𑱍s�F�m�L�[", GetColor(255, 255, 255));

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