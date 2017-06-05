/*
Author:1en_2y
LastModified:2017-6-5 20:46:25
Version:3.0
ProjectName:五子棋
*/
#include<iostream>
#include<cstdio>
#include<conio.h>//使用getche
#include<stdlib.h>
#include<mysql.h>//MYSQL
#include<Windows.h>
#include<string.h>
using namespace std;
#pragma comment (lib, "libmysql.lib") //为了数据库.
#pragma comment (lib, "mysqlclient.lib") //为了数据库
#define SIZE 20//代表棋盘的长和宽
int mysqlflag = 0;//数据库连接成功就为1,没有连接成功就为0,防止showhis函数出错
class board
{
public:
	board();
	void printBoard();//打印棋盘
	void up();//这四个是控制上下左右
	void down();
	void left();
	void right();
	void enter();//下棋操作,不知道英文怎么表示,所以用了个enter;
	int isSet();//检测当前位置是否已经有棋子,如果有,报错.
	int isWin();//判断胜利函数,也是我个人觉得最难写的,约定1代表胜利,0代表还未胜利
	int isOver(int, int);//移动的时候是否已经越界.
	void play();//游戏主函数
	void writesql();//在胜利之后,将游戏结果写入mysql语句
	void showhis();//打印游戏对战历史记录
	void menu();//显示游戏菜单
	MYSQL mysql;//mysql句柄
	MYSQL_RES * result;//mysql结果集
	~board();
	void initbd();
	void updatefield();
private:
	char name1[100], name2[100];//玩家名字
	int map[SIZE][SIZE] = {};//我们约定0代表棋盘位置为空,1代表1玩家的棋子在此,2代表2玩家的棋子在此.
	int cur[2] = { SIZE / 2,SIZE / 2 };//cur代表当前聚焦的位置.初始的时候在棋盘中央.
	int player = 1;//1代表1玩家,2代表2玩家.
	int port;
	char user[20] = "root";//这三个参数请根据自己服务器的情况填写。为了保护我的服务器，我已经将我的密码打码。
	char pswd[20] = "******";
	char ip[20] = "123.207.227.25";//这是我自己的博客，欢迎访问。
};
void board::updatefield()
{
	cout << "input ip, port, user ,password. "<< endl;
	cin >> ip >> port >> user >> pswd;
	mysql_close(&mysql);
	mysql_init(&mysql);
	if (mysql_real_connect(&mysql, ip, user, pswd, NULL, port, NULL, 0))//这里是数据库相关参数
	{
		printf("Connect to remote MySQL server successfully!\n");
		mysqlflag = 1;
		printf("- - - - 1.Play game - - - - -\n");
		printf("- - 2.View history record - -\n");
		printf("- - - 3.Change database - - -\n");
	}
	else
	{
		printf("Cannot connect to remote server. You can play but cannot record your score.\n");
		printf("- - - - 1.Play game - - - - -\n");
		printf("- - 2.View history record - -\n");
		printf("- - - 3.Change database - - -\n");
		mysqlflag = 0;
		return;
	}
	mysql_query(&mysql, "use wuziqi;");
	mysql_query(&mysql, "set names 'gb2312'");//修改字符集,显示中文。
}
board::~board()
{
	mysql_close(&mysql);
}
board::board()
{
	initbd();

	mysql_init(&mysql);
	if (mysql_real_connect(&mysql, ip, user, pswd, NULL, port, NULL, 0))//这里是数据库相关参数
	{
		printf("Connect to remote MySQL server successfully!\n");
		mysqlflag = 1;
	}
	else
	{
		printf("Cannot connect to remote server. You can play but cannot record your score.\n");
	}
	mysql_query(&mysql, "use wuziqi;");
	mysql_query(&mysql, "set names 'gb2312'");//修改字符集,显示中文。

}
void board::initbd()
{
	int a, b;
	for (a = 0; a < SIZE; a++)
	{
		for (b = 0; b < SIZE; b++)
		{
			map[a][b] = 0;
		}//初始化map数组,保证所有的都是0
	}
}
void board::printBoard()
{//☆★○●◎◇◆□■△▲ｘ
	int a, b;
	int n = 1;
	system("cls");
	printf("\n+----------------------------------------+\n");
	for (a = 0; a < SIZE; a++)
	{
		printf("|");//打印边界
		for (b = 0; b < SIZE; b++)
		{
			if (a == cur[0] && b == cur[1])
			{
				printf("ｘ");//如果当前打印的是cur,也就是聚焦的位置,则打印x表示聚焦点,其中x是一个全角字母x
			}
			else
			{
				if (map[a][b] == 0)
				{
					printf("□");//为空打印这个
				}
				else if (map[a][b] == 1)
				{
					printf("●");//一号玩家的棋子标识
				}
				else if (map[a][b] == 2)
				{
					printf("◎");//二号玩家的棋子标识
				}
			}
		}
		printf("|");
		printf("\n");
	}
	printf("+----------------------------------------+\n");
}
void board::right()
{
	if (cur[1] + 1 >= SIZE)
	{
		printf("\nYou have move to border!Cannot move!\n");
		return;
	}
	else
	{
		cur[1]++;
	}
	printBoard();
}
void board::left()
{
	if (cur[1] - 1 < 0)
	{
		printf("\nYou have move to border!Cannot move!\n");
		return;
	}
	else
	{
		cur[1]--;
	}
	printBoard();
}
void board::up()
{
	if (cur[0] - 1 < 0)
	{
		printf("\nYou have move to border!Cannot move!\n");
		return;
	}
	else
	{
		cur[0]--;
	}
	printBoard();
}
void board::down()
{
	if (cur[0] + 1 >= SIZE)
	{
		printf("\nYou have move to border!Cannot move!\n");
		return;
	}
	else
	{
		cur[0]++;
	}
	printBoard();
}
void board::enter()
{
	char ch;
	if (!isSet())//判断是否已经有一个棋子
	{
		map[cur[0]][cur[1]] = player;//player是玩家标识.
	}
	else
	{
		printf("\nCannot set a chessman in there!\n");
		return; //return是为了不进行下面的语句,防止未成功下棋情况下 player标识被更换.
	}
	if (isWin())
	{
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		if (player == 1)
		{
			printf("                                                                                \n                        ############                ###             ###     ### \n                        ###      ###      #######   ###         ########    ### \n                        ###      ###      ### ## ######      #### ##        ### \n                        ###      ###      ### ## ######           ##    ### ### \n                        ###      ###      ### ## ## ###           ##    ### ### \n                        ############      ###### ########## ############### ### \n                ##      ###      ##       ### ##### ###          ###    ### ### \n ##################  ##################   ### ####  ###          ###    ### ### \n                          ###             ### ####  ###         ######  ### ### \n                          ##              #######   ###         ####### ### ### \n                         ############     ##  ## ##########    ##### ###### ### \n                          #      ###      ##  ##    ###        ## ##  ##### ### \n                                 ###      ##  ##    ###       ##  ##    ### ### \n                                 ###      ##  ##    ###      ##   ##        ### \n                                 ###     ###  ##    ###     ##    ##        ### \n                            ###  ##      ## # ##    ###           ##    ##  ### \n                             ######      ## ################      ##     ###### \n                              ####      ##   ##                   ##       ###  \n                                                                                \n");
		}//这一段的printf很长,是为了打印点阵字"一号胜利".
		if (player == 2)
		{
			printf("                                                                                \n                        ############                ###             ###     ### \n                        ###      ###      #######   ###         ########    ### \n                        ###      ###      ### ## ######      #### ##        ### \n               ##       ###      ###      ### ## ######           ##    ### ### \n  ################      ###      ###      ### ## ## ###           ##    ### ### \n                        ############      ###### ########## ############### ### \n                        ###      ##       ### ##### ###          ###    ### ### \n                     ##################   ### ####  ###          ###    ### ### \n                          ###             ### ####  ###         ######  ### ### \n                          ##              #######   ###         ####### ### ### \n                         ############     ##  ## ##########    ##### ###### ### \n                          #      ###      ##  ##    ###        ## ##  ##### ### \n                                 ###      ##  ##    ###       ##  ##    ### ### \n                ##               ###      ##  ##    ###      ##   ##        ### \n ##################              ###     ###  ##    ###     ##    ##        ### \n                            ###  ##      ## # ##    ###           ##    ##  ### \n                             ######      ## ################      ##     ###### \n                              ####      ##   ##                   ##       ###  \n");
		}//这一段的printf很长,是为了打印点阵字"二号胜利".
		writesql();
		printf("\nPress any key to menu.\n");
		system("pause");
		menu();
	}
	if (player == 1)//在一个玩家完成下棋操作以后,将player对调.
	{
		player = 2;
	}
	else
	{
		player = 1;
	}
}
int board::isSet()//0代表此处没有棋子,可以进行下棋操作,1代表不能.
{
	if (map[cur[0]][cur[1]] == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int board::isWin()//首先说明一下 这里的x代表map的第一个参数,y代表map的第二个参数,而x代表的棋盘上对应的竖直方向,y代表棋盘的横向方向.
{
	int a, x, y;//约定这四个为计数变量
	int heng = 1, shu = 1, zuoxie = 1, youxie = 1;//四个方向的棋子数量,之前犯了一个逻辑错误,把这里全部设成0了,其实应该是1,因为下的那个棋子本身就是一个.
	for (a = -1; a > -5; a--)//横向判断开始
	{
		x = cur[0], y = cur[1] + a;//x,y代表坐标,作用是传递给isOver判断是否超出边界
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			heng++;
		}
		else
		{
			break;
		}
	}

	for (a = 1; a < 5; a++)//横向第二阶段
	{
		x = cur[0], y = cur[1] + a;//x,y代表坐标,作用是传递给isOver判断是否超出边界
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			heng++;
		}
		else
		{
			break;
		}
	}

	for (a = -1; a > -5; a--)//竖向判断开始
	{
		x = cur[0] + a, y = cur[1];
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			shu++;
		}
		else
		{
			break;
		}
	}
	for (a = 1; a < 5; a++)//竖向二阶段
	{
		x = cur[0] + a, y = cur[1];
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			shu++;
		}
		else
		{
			break;
		}
	}
	for (a = -1; a > -5; a--)//左斜开始
	{
		x = cur[0] - a, y = cur[1] + a;
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			zuoxie++;
		}
		else
		{
			break;
		}
	}
	for (a = 1; a < 5; a++)//左斜二阶段
	{
		x = cur[0] - a, y = cur[1] + a;
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			zuoxie++;
		}
		else
		{
			break;
		}
	}
	for (a = -1; a > -5; a--)//右斜开始
	{
		x = cur[0] + a, y = cur[1] + a;
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			youxie++;
		}
		else
		{
			break;
		}
	}
	for (a = -1; a > -5; a--)//右斜二阶段
	{
		x = cur[0] - a, y = cur[1] - a;
		if (isOver(x, y))
		{
			break;
		}
		if (map[x][y] == player)
		{
			youxie++;
		}
		else
		{
			break;
		}
	}
	if (heng >= 5 || shu >= 5 || zuoxie >= 5 || youxie >= 5)//四个方向上,有没有大于等于5的
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int board::isOver(int x, int y)//传进来的x,y参数代表即将进行判断的map数组的坐标.返回值约定1代表超出边界,0代表还在边界内.
{
	if (x > SIZE - 1 || x<0 || y>SIZE - 1 || y < 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void board::play()
{
	char ch;
	initbd();
	printf("Welcome! Check you keyboard and ensure lowcase state.wasd to move and x to set a chessman.\nplease input name of player 1 and 2.\n");
	scanf("%s%s", &name1, &name2);

	while (1)
	{
		printBoard();
		if (player == 1)
			printf("\nit's player %d %s 's turn.●\n", player, name1);
		if (player == 2)
			printf("\nit's player %d %s 's turn.◎\n", player, name2);
		ch = getch();
		switch (ch)
		{
		case 'w':up();
			break;
		case 's':down();
			break;
		case 'a':left();
			break;
		case 'd':right();
			break;
		case 'x':enter();
			break;
		}
	}
}
void board::writesql()
{//insert into game values("玩家1","玩家2",NULL);
	char a[1000] = "insert into game values(\"";
	char b[] = "\",\"";
	char c[] = "\", NULL);";//拼接字符串
	if (player == 1) {
		strcat(a, name1);//以下四个strcat是为了拼接字符串,构造出语句 "insert into game values("玩家1","玩家2",NULL);"
		strcat(a, b);
		strcat(a, name2);
		strcat(a, c);
	}
	if (player == 2)
	{
		strcat(a, name2);
		strcat(a, b);
		strcat(a, name1);
		strcat(a, c);
	}
	mysql_query(&mysql, a);//a是构造好的mysql语句,query它
	if (mysqlflag == 0)
	{
		printf("This battle cannot record to database, because you cannot connect to our server, sorry!\n");
	}

}
void board::showhis()
{
	if (mysqlflag == 0)
	{
		printf("This function is unavailable!, because you cannot connect to our server, sorry!\n");
		return;
	}
	MYSQL_RES *my_res;

	MYSQL_ROW my_row;
	int rows, i;
	int res;
	//获取整个表的内容  
	res = mysql_query(&mysql, "select * from game;");
	my_res = mysql_store_result(&mysql);
	//获取表的列数  
	rows = mysql_num_fields(my_res);
	//输出整个表的内容  
	system("cls");
	printf("+---------------+---------------+---------------+\n");
	printf("|Winner         |Loser          |Game ID        |\n");
	printf("+---------------+---------------+---------------+\n");
	while (1)
	{
		my_row = mysql_fetch_row(my_res);
		if (NULL == my_row)
			break;
		for (i = 0; i<rows; i++)
		{
			if (my_row[i] == NULL)
				printf("NULL\t");
			else
				printf("|%-8s\t", (char*)my_row[i]);
		}
		printf("|\n+---------------+---------------+---------------+\n");
	}
	printf("- - - - -1.Play game - - - -\n");
	printf("- - 2.View history record- -\n");
	printf("- - - 3.Change database- - -\n");
}
void board::menu()
{

	char ch;
	printf("* * * * * * * * * * * * * * *\n");
	printf("- - - - - -!WELCOME!- - - - -\n");
	printf("- - - - 1.Play game - - - - -\n");
	printf("- - 2.View history record - -\n");
	printf("- - - 3.Change database - - -\n");
	printf("- - Press 1 2 3 to choose - -\n");
	printf("* * * * * * * * * * * * * * *\n");
	while (1)
	{
		ch = getch();
		switch (ch)
		{
		case '1':
			play();
			break;
		case '2':
			showhis();
			break;
		case '3':
			updatefield();
			break;
		default:
			printf("Invalid value!Input again!\n");
			break;
		}
	}
}
int main()
{
	board *game1;
	game1 = new board;
	game1->menu();
	return 0;
}