#include"kunkun.h"
#include <stdlib.h>
#define Max 3
enum  date
{
	space,//0
	wall,//1
	kunkun,//2
	ball,//3
	dest//41
};
char * img[] = { "./res/00.bmp","./res/01.bmp","./res/02.bmp","./res/03.bmp","./res/04.bmp","./res/05.bmp" };
char *kb[] = { "./res/up.bmp","./res/down.bmp","./res/left.bmp","./res/right.bmp","./res/restart.bmp","./res/exit.bmp"};
int map[Max][10][10] = {
		{1,1,1,1,1,1,1,1,1,1,
		 1,2,0,0,1,1,1,1,1,1,
		 1,0,3,3,1,1,1,1,1,1,
		 1,0,3,0,1,1,1,4,1,1,
		 1,1,1,0,1,1,1,4,1,1,
		 1,1,1,0,0,0,0,4,1,1,
		 1,1,0,0,0,1,0,0,1,1,
		 1,1,0,0,0,1,1,1,1,1,
		 1,1,1,1,1,1,1,1,1,1,
		 1,1,1,1,1,1,1,1,1,1
	   },
	{
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,2,1,1,
		1,1,1,1,0,0,3,0,1,1,
		1,1,1,1,0,1,0,1,1,1,
		1,1,1,0,0,1,0,1,4,1,
		1,1,1,0,1,0,0,3,4,1,
		1,1,1,0,3,0,0,0,4,1,
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1
	},
	{
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,0,0,0,1,1,1,1,
		1,4,2,3,0,0,1,1,1,1,
		1,1,1,0,3,4,1,1,1,1,
		1,4,1,1,3,0,1,1,1,1,
		1,0,1,0,4,0,1,1,1,1,
		1,3,0,7,3,3,4,1,1,1,
		1,0,0,0,4,0,0,1,1,1,
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1
	}
};
int tempmap[Max][10][10] = {
		{1,1,1,1,1,1,1,1,1,1,
		 1,2,0,0,1,1,1,1,1,1,
		 1,0,3,3,1,1,1,1,1,1,
		 1,0,3,0,1,1,1,4,1,1,
		 1,1,1,0,1,1,1,4,1,1,
		 1,1,1,0,0,0,0,4,1,1,
		 1,1,0,0,0,1,0,0,1,1,
		 1,1,0,0,0,1,1,1,1,1,
		 1,1,1,1,1,1,1,1,1,1,
		 1,1,1,1,1,1,1,1,1,1
	   },
	{
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,2,1,1,
		1,1,1,1,0,0,3,0,1,1,
		1,1,1,1,0,1,0,1,1,1,
		1,1,1,0,0,1,0,1,4,1,
		1,1,1,0,1,0,0,3,4,1,
		1,1,1,0,3,0,0,0,4,1,
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1
	},
	{
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,0,0,0,1,1,1,1,
		1,4,2,3,0,0,1,1,1,1,
		1,1,1,0,3,4,1,1,1,1,
		1,4,1,1,3,0,1,1,1,1,
		1,0,1,0,4,0,1,1,1,1,
		1,3,0,7,3,3,4,1,1,1,
		1,0,0,0,4,0,0,1,1,1,
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1
	}
};
int Exit = 0;
void drawmap(int map[][10][10], int cur)
{
	int i, j;
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10; j++)
		{
			drawppic(map[cur][i][j], i, j);
		}
		// putchar('\n');
	}
}
void drawppic(int k,int i,int j) {
	if (i < 0 || i >= 10 || j < 0 || j >= 10)return;
	switch (k)
	{
	case space:
		bmp(j * 48 + 320, i * 48, img[0]);
		break;
	case wall:
		bmp( j * 48 + 320, i * 48, img[1]);
		break;
	case kunkun:
	case kunkun + dest:
			bmp( j * 48 + 320, i * 48, img[2]);
		break;
	case ball:
		bmp(  j * 48 + 320, i * 48, img[3]);
		break;
	case dest:
		bmp(j * 48 + 320, i * 48, img[4]);
		break;
	case ball + dest:
		bmp( j * 48 + 320, i * 48, img[5]);
	default:
		break;
	}
}
//地图绘制
void keycontral(int map[][10][10], int cur, int tempmap[][10][10])
{
	int i, j, x, y;
	char key;
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (map[cur][i][j] == kunkun || map[cur][i][j] == kunkun + dest)
			{
				break;
			}
		}
		if (map[cur][i][j] == kunkun || map[cur][i][j] == kunkun + dest)
		{
			break;
		}
	}//找到坤坤坐标
	get_touch(&x, &y);
	if (x >= 5 && x <= 105 && y >= 370 && y <= 470) { key = 'A'; }
	else if (x >= 215 && x <= 315 && y >= 370 && y <= 470) { key = 'D'; }
	else if (x >= 110 && x <= 210 && y >= 370 && y <= 480) { key = 'S'; }
	else if (x >= 110 && x <= 210 && y >= 270 && y <= 370) { key = 'W';  }
	else if (x >= 200 && x <= 300 && y >= 5 && y <= 105) { key = '\r'; }
	else if (x >= 0 && x <= 100 && y >= 0 && y <= 100) { Exit = 1; return; }
	switch (key)
	{
	case 'W':
		if (map[cur][i - 1][j] == space || map[cur][i - 1][j] == dest)
		{
			map[cur][i - 1][j] += kunkun;
			map[cur][i][j] -= kunkun;
		}
		if (map[cur][i - 1][j] == ball || map[cur][i - 1][j] == dest + ball)
		{
			if (map[cur][i - 2][j] == space || map[cur][i - 2][j] == dest)
			{
				map[cur][i - 2][j] += ball;
				map[cur][i - 1][j] -= ball;
				map[cur][i - 1][j] += kunkun;
				map[cur][i][j] -= kunkun;

			}
		}
		drawppic(map[cur][i - 2][j], i - 2, j);
		drawppic(map[cur][i - 1][j], i - 1, j);
		drawppic(map[cur][i][j], i, j);
		break;
	case 'S':
		if (map[cur][i + 1][j] == space || map[cur][i + 1][j] == dest)
		{
			map[cur][i + 1][j] += kunkun;
			map[cur][i][j] -= kunkun;
		}
		if (map[cur][i + 1][j] == ball || map[cur][i + 1][j] == dest + ball)
		{
			if (map[cur][i + 2][j] == space || map[cur][i + 2][j] == dest)
			{
				map[cur][i + 2][j] += ball;
				map[cur][i + 1][j] -= ball;
				map[cur][i + 1][j] += kunkun;
				map[cur][i][j] -= kunkun;
			}
		}
		drawppic(map[cur][i + 2][j], i + 2, j);
		drawppic(map[cur][i + 1][j], i + 1, j);
		drawppic(map[cur][i][j], i, j);
		break;
	case 'A':
		if (map[cur][i][j - 1] == space || map[cur][i][j - 1] == dest)
		{
			map[cur][i][j - 1] += kunkun;
			map[cur][i][j] -= kunkun;
		}
		if (map[cur][i][j - 1] == ball || map[cur][i][j - 1] == dest + ball)
		{
			if (map[cur][i][j - 2] == space || map[cur][i][j - 2] == dest)
			{
				map[cur][i][j - 2] += ball;
				map[cur][i][j - 1] -= ball;
				map[cur][i][j - 1] += kunkun;
				map[cur][i][j] -= kunkun;
			}
		}
		drawppic(map[cur][i][j - 2], i, j - 2);
		drawppic(map[cur][i][j - 1], i, j - 1);
		drawppic(map[cur][i][j], i, j);
		break;
	case 'D':
		if (map[cur][i][j + 1] == space || map[cur][i][j + 1] == dest)
		{
			map[cur][i][j + 1] += kunkun;
			map[cur][i][j] -= kunkun;
		}
		if (map[cur][i][j + 1] == ball || map[cur][i][j + 1] == dest + ball)
		{
			if (map[cur][i][j + 2] == space || map[cur][i][j + 2] == dest)
			{
				map[cur][i][j + 2] += ball;
				map[cur][i][j + 1] -= ball;
				map[cur][i][j + 1] += kunkun;
				map[cur][i][j] -= kunkun;

			}
		}
		drawppic(map[cur][i][j + 2], i, j + 2);
		drawppic(map[cur][i][j + 1], i, j + 1);
		drawppic(map[cur][i][j], i, j);
		break;
	case '\r':
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 10; j++)
			{
				map[cur][i][j] = tempmap[cur][i][j];
			}

		}
		drawmap(map, cur);
		break;
	}

}
//控制判断
int ifpass(int map[][10][10], int cur)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (map[cur][i][j] == ball)
			{
				return 1;
			}
		}
	}
	return 0;
}
//游戏结束判断
void drawlog() {
	bmp2(5, 480-110, kb[2]);
	bmp2(110, 480-90, kb[1]);
	bmp2(110, 480-210, kb[0]);
	bmp2(215, 480-110, kb[3]);
	bmp2(200, 5, kb[4]);
	bmp2(0, 0, kb[5]);
}
int main()
{
	lcd_init();
	int cur = 0;
	int score = 0;
	int i = 0;
	drawmap(map, cur);
	drawlog();
	while (1) {
        int key = ifpass(map, cur);
        if (key == 0) {
            cur = (cur + 1) % Max;
            key = 1;
            score++;
            drawmap(map, cur);
        };
        keycontral(map, cur, tempmap);
        if (Exit) {
            system("./main");
            exit(0);
        }
    }
}