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


enum GAME_SCENE {
	GAME_SCENE_TITLE,		//�^�C�g�����
	GAME_SCENE_RANKING,		//�����L���O���
	GAME_SCENE_SOZAI,		//�f�ޕ\�����
	GAME_SCENE_PLAY_ENDLESS,//�v���C���(�G���h���X���[�h)
	GAME_SCENE_PLAY_TIME,	//�v���C���(�^�C���A�^�b�N���[�h)
	GAME_SCENE_CHECK_ENDLESS,		//�m�F���
	GAME_SCENE_CHECK_TIME,		//�m�F���
	GAME_SCENE_END_CLEAR,	//�G���h���(�Q�[���N���A)
	GAME_SCENE_END_OVER		//�G���h���(�Q�[���I�[�o�[)
};


BOOL IsWM_CREATE = FALSE;	//WM_CREATE������ɓ��삵�������f����

int StartTimeFps;					//����J�n����
int CountFps;						//�J�E���^
float CalcFps;						//�v�Z����
int SampleNumFps = GAME_FPS_SPEED;	//���ς����T���v����

char AllKeyState[256];	//�S�ẴL�[�̏�Ԃ�����

int GameSceneNow = (int)GAME_SCENE_TITLE;	//�ŏ��̃Q�[����ʂ��^�C�g���ɐݒ�


LRESULT CALLBACK MY_WNDPROC(HWND, UINT, WPARAM, LPARAM);	//����E�B���h�E�v���V�[�W��

VOID MY_FPS_UPDATE(VOID);	//FPS���v���A�X�V����֐�
VOID MY_FPS_WAIT(VOID);		//FPS�l���v�����A�҂֐�

VOID MY_ALL_KEYDOWN_UPDATE(VOID);	//�L�[�̓��͏�Ԃ��X�V����֐�

VOID MY_GAME_TITLE(VOID);		//�^�C�g��
VOID MY_GAME_RANKING(VOID);		//�����L���O
VOID MY_GAME_SOZAI(VOID);		//�f�ޕ\��
VOID MY_GAME_PLAY_ENDLESS(VOID);//�v���C�E�G���h���X
VOID MY_GAME_PLAY_TIME(VOID);	//�v���C�E�^�C���A�^�b�N
VOID MY_GAME_CHECK_ENDLESS(VOID);		//�m�F
VOID MY_GAME_CHECK_TIME(VOID);		//�m�F
VOID MY_GAME_END_CLEAR(VOID);	//�Q�[���N���A
VOID MY_GAME_END_OVER(VOID);	//�Q�[���I�[�o�[

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(GAME_WINDOW_MODECHANGE);	//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŉ�ʂ�\������

	SetWindowStyleMode(SET_WINDOW_ST_MODE_DEFAULT);	//�^�C�g���o�[

	SetMainWindowText(TEXT(GAME_WINDOW_NAME));		//�^�C�g���̕���

	if (DxLib_Init() == -1)	{ return -1; }	//DxLib����������

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

VOID MY_GAME_TITLE(VOID)
{
	if (AllKeyState[KEY_INPUT_E] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}
	else if (AllKeyState[KEY_INPUT_T] != 0)
	{
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

	DrawString(0, 0, "�^�C�g�����", GetColor(255, 255, 255));
	DrawString(0, 30, "�G���h���X���[�h�F�d�L�[", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C���A�^�b�N���[�h�F�s�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�����L���O�F�q�L�[", GetColor(255, 255, 255));
	DrawString(0, 120, "�f�ށF�r�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_RANKING(VOID)
{
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	DrawString(0, 0, "�����L���O���", GetColor(255, 255, 255));
	DrawString(0, 30, "�^�C�g���ɖ߂�F�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_SOZAI(VOID)
{
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}

	DrawString(0, 0, "�f�ޕ\�����", GetColor(255, 255, 255));
	DrawString(0, 30, "�^�C�g���ɖ߂�F�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_PLAY_ENDLESS(VOID)
{
	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_END_OVER;
	}
	else if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_CHECK_ENDLESS;
	}

	DrawString(0, 0, "�v���C���(�G���h���X���[�h)", GetColor(255, 255, 255));
	DrawString(0, 30, "�G���h��ʁF�d�m�s�d�q�L�[", GetColor(255, 255, 255));
	DrawString(0, 60, "�m�F��ʁF�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_PLAY_TIME(VOID)
{
	if (AllKeyState[KEY_INPUT_RETURN] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_END_OVER;
	}
	else if (AllKeyState[KEY_INPUT_SPACE] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_END_CLEAR;
	}
	else if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_CHECK_TIME;
	}

	DrawString(0, 0, "�v���C���(�^�C���A�^�b�N���[�h)", GetColor(255, 255, 255));
	DrawString(0, 30, "�G���h���(�Q�[���I�[�o�[)�F�d�m�s�d�q�L�[", GetColor(255, 255, 255));
	DrawString(0, 60, "�G���h���(�Q�[���N���A)�F�r�o�`�b�d�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�m�F��ʁF�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_CHECK_ENDLESS(VOID)
{
	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_N] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY_ENDLESS;
	}

	DrawString(0, 0, "�m�F���(�G���h���X)", GetColor(255, 255, 255));
	DrawString(0, 30, "�Q�[�����I�����܂����H", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C�g���F�x�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�v���C�𑱍s�F�m�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_CHECK_TIME(VOID)
{
	if (AllKeyState[KEY_INPUT_Y] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_N] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_PLAY_TIME;
	}

	DrawString(0, 0, "�m�F���(�G���h���X)", GetColor(255, 255, 255));
	DrawString(0, 30, "�Q�[�����I�����܂����H", GetColor(255, 255, 255));
	DrawString(0, 60, "�^�C�g���F�x�L�[", GetColor(255, 255, 255));
	DrawString(0, 90, "�v���C�𑱍s�F�m�L�[", GetColor(255, 255, 255));

	return;
}

VOID MY_GAME_END_OVER(VOID)
{
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}

	DrawString(0, 0, "�G���h���(�Q�[���I�[�o�[)", GetColor(255, 255, 255));
	DrawString(0, 30, "�^�C�g���F�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));
	DrawString(0, 60, "�����L���O�F�q�L�[", GetColor(255, 255, 255));
	return;
}

VOID MY_GAME_END_CLEAR(VOID)
{
	if (AllKeyState[KEY_INPUT_BACK] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_TITLE;
	}
	else if (AllKeyState[KEY_INPUT_R] != 0)
	{
		GameSceneNow = (int)GAME_SCENE_RANKING;
	}
	
	DrawString(0, 0, "�G���h���(�Q�[���N���A)", GetColor(255, 255, 255));
	DrawString(0, 30, "�^�C�g���F�a�`�b�j�r�o�`�b�d�L�[", GetColor(255, 255, 255));
	DrawString(0, 60, "�����L���O�F�q�L�[", GetColor(255, 255, 255));

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

		MessageBox(hwnd, TEXT("�Q�[�����I�����܂�"), TEXT("�I�����b�Z�[�W"), MB_OK);
		break;

	case WM_RBUTTONDOWN:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
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