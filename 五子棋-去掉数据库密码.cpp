/*
Author:1en_2y
LastModified:2017-6-5 20:46:25
Version:3.0
ProjectName:������
*/
#include<iostream>
#include<cstdio>
#include<conio.h>//ʹ��getche
#include<stdlib.h>
#include<mysql.h>//MYSQL
#include<Windows.h>
#include<string.h>
using namespace std;
#pragma comment (lib, "libmysql.lib") //Ϊ�����ݿ�.
#pragma comment (lib, "mysqlclient.lib") //Ϊ�����ݿ�
#define SIZE 20//�������̵ĳ��Ϳ�
int mysqlflag = 0;//���ݿ����ӳɹ���Ϊ1,û�����ӳɹ���Ϊ0,��ֹshowhis��������
class board
{
public:
	board();
	void printBoard();//��ӡ����
	void up();//���ĸ��ǿ�����������
	void down();
	void left();
	void right();
	void enter();//�������,��֪��Ӣ����ô��ʾ,�������˸�enter;
	int isSet();//��⵱ǰλ���Ƿ��Ѿ�������,�����,����.
	int isWin();//�ж�ʤ������,Ҳ���Ҹ��˾�������д��,Լ��1����ʤ��,0����δʤ��
	int isOver(int, int);//�ƶ���ʱ���Ƿ��Ѿ�Խ��.
	void play();//��Ϸ������
	void writesql();//��ʤ��֮��,����Ϸ���д��mysql���
	void showhis();//��ӡ��Ϸ��ս��ʷ��¼
	void menu();//��ʾ��Ϸ�˵�
	MYSQL mysql;//mysql���
	MYSQL_RES * result;//mysql�����
	~board();
	void initbd();
	void updatefield();
private:
	char name1[100], name2[100];//�������
	int map[SIZE][SIZE] = {};//����Լ��0��������λ��Ϊ��,1����1��ҵ������ڴ�,2����2��ҵ������ڴ�.
	int cur[2] = { SIZE / 2,SIZE / 2 };//cur����ǰ�۽���λ��.��ʼ��ʱ������������.
	int player = 1;//1����1���,2����2���.
	int port;
	char user[20] = "root";//����������������Լ��������������д��Ϊ�˱����ҵķ����������Ѿ����ҵ�������롣
	char pswd[20] = "******";
	char ip[20] = "123.207.227.25";//�������Լ��Ĳ��ͣ���ӭ���ʡ�
};
void board::updatefield()
{
	cout << "input ip, port, user ,password. "<< endl;
	cin >> ip >> port >> user >> pswd;
	mysql_close(&mysql);
	mysql_init(&mysql);
	if (mysql_real_connect(&mysql, ip, user, pswd, NULL, port, NULL, 0))//���������ݿ���ز���
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
	mysql_query(&mysql, "set names 'gb2312'");//�޸��ַ���,��ʾ���ġ�
}
board::~board()
{
	mysql_close(&mysql);
}
board::board()
{
	initbd();

	mysql_init(&mysql);
	if (mysql_real_connect(&mysql, ip, user, pswd, NULL, port, NULL, 0))//���������ݿ���ز���
	{
		printf("Connect to remote MySQL server successfully!\n");
		mysqlflag = 1;
	}
	else
	{
		printf("Cannot connect to remote server. You can play but cannot record your score.\n");
	}
	mysql_query(&mysql, "use wuziqi;");
	mysql_query(&mysql, "set names 'gb2312'");//�޸��ַ���,��ʾ���ġ�

}
void board::initbd()
{
	int a, b;
	for (a = 0; a < SIZE; a++)
	{
		for (b = 0; b < SIZE; b++)
		{
			map[a][b] = 0;
		}//��ʼ��map����,��֤���еĶ���0
	}
}
void board::printBoard()
{//������������������
	int a, b;
	int n = 1;
	system("cls");
	printf("\n+----------------------------------------+\n");
	for (a = 0; a < SIZE; a++)
	{
		printf("|");//��ӡ�߽�
		for (b = 0; b < SIZE; b++)
		{
			if (a == cur[0] && b == cur[1])
			{
				printf("��");//�����ǰ��ӡ����cur,Ҳ���Ǿ۽���λ��,���ӡx��ʾ�۽���,����x��һ��ȫ����ĸx
			}
			else
			{
				if (map[a][b] == 0)
				{
					printf("��");//Ϊ�մ�ӡ���
				}
				else if (map[a][b] == 1)
				{
					printf("��");//һ����ҵ����ӱ�ʶ
				}
				else if (map[a][b] == 2)
				{
					printf("��");//������ҵ����ӱ�ʶ
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
	if (!isSet())//�ж��Ƿ��Ѿ���һ������
	{
		map[cur[0]][cur[1]] = player;//player����ұ�ʶ.
	}
	else
	{
		printf("\nCannot set a chessman in there!\n");
		return; //return��Ϊ�˲�������������,��ֹδ�ɹ���������� player��ʶ������.
	}
	if (isWin())
	{
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		if (player == 1)
		{
			printf("                                                                                \n                        ############                ###             ###     ### \n                        ###      ###      #######   ###         ########    ### \n                        ###      ###      ### ## ######      #### ##        ### \n                        ###      ###      ### ## ######           ##    ### ### \n                        ###      ###      ### ## ## ###           ##    ### ### \n                        ############      ###### ########## ############### ### \n                ##      ###      ##       ### ##### ###          ###    ### ### \n ##################  ##################   ### ####  ###          ###    ### ### \n                          ###             ### ####  ###         ######  ### ### \n                          ##              #######   ###         ####### ### ### \n                         ############     ##  ## ##########    ##### ###### ### \n                          #      ###      ##  ##    ###        ## ##  ##### ### \n                                 ###      ##  ##    ###       ##  ##    ### ### \n                                 ###      ##  ##    ###      ##   ##        ### \n                                 ###     ###  ##    ###     ##    ##        ### \n                            ###  ##      ## # ##    ###           ##    ##  ### \n                             ######      ## ################      ##     ###### \n                              ####      ##   ##                   ##       ###  \n                                                                                \n");
		}//��һ�ε�printf�ܳ�,��Ϊ�˴�ӡ������"һ��ʤ��".
		if (player == 2)
		{
			printf("                                                                                \n                        ############                ###             ###     ### \n                        ###      ###      #######   ###         ########    ### \n                        ###      ###      ### ## ######      #### ##        ### \n               ##       ###      ###      ### ## ######           ##    ### ### \n  ################      ###      ###      ### ## ## ###           ##    ### ### \n                        ############      ###### ########## ############### ### \n                        ###      ##       ### ##### ###          ###    ### ### \n                     ##################   ### ####  ###          ###    ### ### \n                          ###             ### ####  ###         ######  ### ### \n                          ##              #######   ###         ####### ### ### \n                         ############     ##  ## ##########    ##### ###### ### \n                          #      ###      ##  ##    ###        ## ##  ##### ### \n                                 ###      ##  ##    ###       ##  ##    ### ### \n                ##               ###      ##  ##    ###      ##   ##        ### \n ##################              ###     ###  ##    ###     ##    ##        ### \n                            ###  ##      ## # ##    ###           ##    ##  ### \n                             ######      ## ################      ##     ###### \n                              ####      ##   ##                   ##       ###  \n");
		}//��һ�ε�printf�ܳ�,��Ϊ�˴�ӡ������"����ʤ��".
		writesql();
		printf("\nPress any key to menu.\n");
		system("pause");
		menu();
	}
	if (player == 1)//��һ����������������Ժ�,��player�Ե�.
	{
		player = 2;
	}
	else
	{
		player = 1;
	}
}
int board::isSet()//0����˴�û������,���Խ����������,1������.
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
int board::isWin()//����˵��һ�� �����x����map�ĵ�һ������,y����map�ĵڶ�������,��x����������϶�Ӧ����ֱ����,y�������̵ĺ�����.
{
	int a, x, y;//Լ�����ĸ�Ϊ��������
	int heng = 1, shu = 1, zuoxie = 1, youxie = 1;//�ĸ��������������,֮ǰ����һ���߼�����,������ȫ�����0��,��ʵӦ����1,��Ϊ�µ��Ǹ����ӱ������һ��.
	for (a = -1; a > -5; a--)//�����жϿ�ʼ
	{
		x = cur[0], y = cur[1] + a;//x,y��������,�����Ǵ��ݸ�isOver�ж��Ƿ񳬳��߽�
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

	for (a = 1; a < 5; a++)//����ڶ��׶�
	{
		x = cur[0], y = cur[1] + a;//x,y��������,�����Ǵ��ݸ�isOver�ж��Ƿ񳬳��߽�
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

	for (a = -1; a > -5; a--)//�����жϿ�ʼ
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
	for (a = 1; a < 5; a++)//������׶�
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
	for (a = -1; a > -5; a--)//��б��ʼ
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
	for (a = 1; a < 5; a++)//��б���׶�
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
	for (a = -1; a > -5; a--)//��б��ʼ
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
	for (a = -1; a > -5; a--)//��б���׶�
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
	if (heng >= 5 || shu >= 5 || zuoxie >= 5 || youxie >= 5)//�ĸ�������,��û�д��ڵ���5��
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int board::isOver(int x, int y)//��������x,y���������������жϵ�map���������.����ֵԼ��1�������߽�,0�����ڱ߽���.
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
			printf("\nit's player %d %s 's turn.��\n", player, name1);
		if (player == 2)
			printf("\nit's player %d %s 's turn.��\n", player, name2);
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
{//insert into game values("���1","���2",NULL);
	char a[1000] = "insert into game values(\"";
	char b[] = "\",\"";
	char c[] = "\", NULL);";//ƴ���ַ���
	if (player == 1) {
		strcat(a, name1);//�����ĸ�strcat��Ϊ��ƴ���ַ���,�������� "insert into game values("���1","���2",NULL);"
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
	mysql_query(&mysql, a);//a�ǹ���õ�mysql���,query��
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
	//��ȡ�����������  
	res = mysql_query(&mysql, "select * from game;");
	my_res = mysql_store_result(&mysql);
	//��ȡ�������  
	rows = mysql_num_fields(my_res);
	//��������������  
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