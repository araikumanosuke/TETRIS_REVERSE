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

#define GAME_IMAGE_BG_TITLE			"IMAGE\\bg_title.png"
#define GAME_IMAGE_BG_RANK			"IMAGE\\bg_rank.png"
#define GAME_IMAGE_BG_PLAY_ENDLESS	"IMAGE\\bg_play_endless.png"
#define GAME_IMAGE_BG_PLAY_TIME		"IMAGE\\bg_play_time.png"
#define GAME_IMAGE_BG_END_OVER		"IMAGE\\bg_end_over.png"
#define GAME_IMAGE_BG_END_CLEAR		"IMAGE\\bg_end_clear.png"


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

BOOL IsWM_CREATE = FALSE;	//WM_CREATE������ɓ��삵�������f����

int StartTimeFps;					//����J�n����
int CountFps;						//�J�E���^
float CalcFps;						//�v�Z����
int SampleNumFps = GAME_FPS_SPEED;	//���ς����T���v����

char AllKeyState[256];	//�S�ẴL�[�̏�Ԃ�����

int GameSceneNow = (int)GAME_SCENE_TITLE;	//�ŏ��̃Q�[����ʂ��^�C�g���ɐݒ�

int syoki_flag = 0;

int etc_fonthandle;
int Go_fonthandle;

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

LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);	//����E�B���h�E�v���V�[�W��

VOID MY_FPS_UPDATE(VOID);	//FPS���v���A�X�V����֐�
VOID MY_FPS_WAIT(VOID);		//FPS�l���v�����A�҂֐�

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//�L�[�̓��͏�Ԃ��X�V����֐�

VOID MY_SOUND_LOAD(SOUND *,const char *);

VOID MY_IMAGE_LOAD(IMAGE *, int, int, const char *);

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

	if (DxLib_Init() == -1)	{ return -1; }	//DxLib����������

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

	etc_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B", 60, -1, DX_FONTTYPE_ANTIALIASING);
	Go_fonthandle = CreateFontToHandle("UD �f�W�^�� ���ȏ��� NK-B", 120, -1, DX_FONTTYPE_ANTIALIASING);

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

	DeleteFontToHandle(etc_fonthandle);
	DeleteFontToHandle(Go_fonthandle);

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

VOID MY_GAME_TITLE(VOID)
{
	syoki_flag = 0;

	if (CheckSoundMem(bgm_title_etc.handle) == 0)
	{
		PlaySoundMem(bgm_title_etc.handle, DX_PLAYTYPE_LOOP);
	}

	DrawGraph(bg_title.x, bg_title.y, bg_title.handle, TRUE);

	if (AllKeyState[KEY_INPUT_E] != 0)
	{
		StopSoundMem(bgm_title_etc.handle);
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}
	else if (AllKeyState[KEY_INPUT_T] != 0)
	{
		StopSoundMem(bgm_title_etc.handle);
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
		PlaySoundMem(bgm_title_etc.handle, DX_PLAYTYPE_LOOP);
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
		PlaySoundMem(bgm_title_etc.handle, DX_PLAYTYPE_LOOP);
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
	
	static int syoki_temp;

	int syoki_count = GetNowCount();

	if (syoki_flag == 0)
	{
		syoki_temp = syoki_count;
		syoki_flag = 1;
	}
	
	if (syoki_count - syoki_temp < 1000)
	{
		DrawStringToHandle(150, 250, "Ready...", GetColor(255, 255, 255),etc_fonthandle);
	}
	else if (syoki_count - syoki_temp > 1001 && syoki_count - syoki_temp < 2000)
	{
		DrawStringToHandle(175, 400, "Go!", GetColor(255, 255, 255), Go_fonthandle);
	}
	else if (syoki_count - syoki_temp > 2000)
	{
		if (CheckSoundMem(bgm_play.handle) == 0)
		{
			PlaySoundMem(bgm_play.handle, DX_PLAYTYPE_LOOP);
		}
	}


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
	DrawGraph(bg_play_time.x, bg_play_time.y, bg_play_time.handle, TRUE);
	
	if (CheckSoundMem(bgm_play.handle) == 0)
	{
		PlaySoundMem(bgm_play.handle, DX_PLAYTYPE_LOOP);
	}

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
	SetFontSize(24);
	DrawString(0, 0, "�m�F���(�G���h���X)", GetColor(255, 255, 255));
	DrawString(0, 30, "�Q�[�����I�����܂����H", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C�g���F�x�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�v���C�𑱍s�F�m�L�[", GetColor(255, 255, 255));

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
	SetFontSize(24);
	DrawString(0, 0, "�m�F���(�G���h���X)", GetColor(255, 255, 255));
	DrawString(0, 30, "�Q�[�����I�����܂����H", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C�g���F�x�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�v���C�𑱍s�F�m�L�[", GetColor(255, 255, 255));

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
	if (CheckSoundMem(bgm_end_over.handle) == 0)
	{
		PlaySoundMem(bgm_end_over.handle, DX_PLAYTYPE_LOOP);
	}
	
	DrawGraph(bg_end_over.x, bg_end_over.y, bg_end_over.handle, TRUE);
	
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		StopSoundMem(bgm_end_over.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		StopSoundMem(bgm_end_over.handle);
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}

	return;
}

VOID MY_GAME_END_CLEAR(VOID)
{
	if (CheckSoundMem(bgm_end_clear.handle) == 0)
	{
		PlaySoundMem(bgm_end_clear.handle, DX_PLAYTYPE_LOOP);
	}
	
	DrawGraph(bg_end_clear.x, bg_end_clear.y, bg_end_clear.handle, TRUE);

	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		StopSoundMem(bgm_end_clear.handle);
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		StopSoundMem(bgm_end_clear.handle);
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