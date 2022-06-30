#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <fstream>
#define BUFSIZE 20
using namespace std;
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); // получаем дескриптор консоли

typedef struct account
{
	char login[BUFSIZE];
	char password[BUFSIZE]; //beresten
	int status; //постоянное значение, менять которое может только администратор 
	account* next_account;
}Account;
typedef struct employee
{
	char name[BUFSIZE];
	char secound_name[BUFSIZE];
	char surname[BUFSIZE]; //здесь: отчество
	int salary_per_day; //ставка
}Employee;
typedef struct sick_list
{
	int num_of_list;
	int year;
	int mounth;
	int days_count;
	employee Employee;
	int payment; //выплаты
	sick_list* next_list;
}Sick_list;

int list_count = 0; // количество больничных листов
int account_count = 0; //количество учётных записей
sick_list* head_list = NULL; //начало списка больничных листов
sick_list* TEMP_LIST = NULL; //временное значение для списка больничных листов
account* TEMP_ACCOUNT = NULL; //временное значение для списка аккаунтов
account* head_account = NULL; //начало списка аккаунтов
const char key[] = { "list_key" }; //константа для гаммирования
int z = 0; //переменная для контроля курсора

sick_list* getNlist(sick_list* head, int n); //перемещение указателя на n-ый узел списка больничных листов
account* getNaccount(account* head, int n); //перемещение указателя на n-ый узел списка аккаунтов
bool isEmpty(int record_count); //проверка: пустой ли список
int wish_sick_list(); //выбор больничного листа из списка с помощью клавиш "вверх" "вниз"
int wish_account(); //выбор аккаунта из списка с помощью клавиш "вверх" "вниз"

//две функции, управляющие курсором и расположением текста в консоли
void GoToXY(short x, short y);
void ConsoleCursorVisible(bool show, short size);

void secretpass(char mas[BUFSIZE]); //функция ввода пароля, контролирующая ввод и заменяющая символы на экране на '*'
int encryption(char code[]); //шифрование пароля
int decryption(char cipher[]); //расшифровка пароля

int intread(int t);//ввод целочисленного значения
int yearread();//проверка на ввод года
int mounthread();//проверка на ввод месяца
int dayscountread(int mounth, int year);//проверка на ввод количества дней

//сортировки данных больничных листов
void sort_num_of_list();
void sort_year();
void sort_mounth();
void sort_days_count();
void sort_name();
void sort_secound_name();
void sort_salary_per_day();
void sort_payment();
bool sort_data_menu(int x, int y, int active_menu, bool exit);

//сортировка учётных записей по логину и статусу
void sort_login();
void sort_status();
bool sort_account_menu(int x, int y, int active_menu, bool exit);

void individual_task(); //выполнение индивидуального задания
bool search_menu(int x, int y, int active_menu, bool exit);

void print_list(); //печать больничного листа
int delete_first(sick_list** head); //удаление первого больничного листа
int delNlist(sick_list** head, int n); //удаление больничного листа
sick_list* list_from_file(); //извлечение больничных листов из файла
void list_to_file(); //запись больничных листов в файл

void print_users(); //печать списка аккаунтов
int delAccount(account** head, int n); //удаление аккаунта
account* account_from_file(); //извлечение списка аккаунтов из файла
void account_to_file(); //запись списка аккаунта в файл

//меню, отвечающие за удобство исполнения программы
bool menu(int x, int y, int active_menu, bool exit);
bool Admin_account_menu(int x, int y, int active_menu, bool exit);
bool Admin_data_menu(int x, int y, int active_menu, bool exit);
bool Admin_menu(int x, int y, int active_menu, bool exit);
bool User_data_menu(int x, int y, int active_menu, bool exit);

bool function(string* menu_arr, int menu_lenth, bool (*func)(int x, int y, int active_menu, bool exit)); //общая функция, которая позволяет оформить интерактивное меню для различных случаев
void login(); //авторизация

int main()
{
	SetConsoleTitle(L"Расчет выплат по больничным листам");
	system("CLS");
	setlocale(LC_ALL, "rus");
	ConsoleCursorVisible(false, 100);
	account_from_file();
	string Main_menu[] = { "Войти в аккаунт", "Выйти из программы" };
	function(Main_menu, 1, menu);
	account_to_file();
}

sick_list* getNlist(sick_list* head, int n) //Функция возвращает указатель на n-ый узел списка
{
	int counter = 1;
	while (counter < n && head) {
		head = head->next_list;
		counter++;
	}
	return head;
}
account* getNaccount(account* head, int n)
{
	int counter = 1;
	while (counter < n && head) {
		head = head->next_account;
		counter++;
	}
	return head;
}
bool isEmpty(int record_count) //возвращает 1, если список пустой
{
	if (record_count == 0)
	{
		printf("\n_______________________________________________");
		printf("\nНет записей!\n");
		system("pause");
		return 1;
	}
	else return 0;
}
int wish_sick_list()	//функция выбора нужного больничного листа из списка с помощью клавиш "Вверх" "Вниз"
{
	char active_bottom;
	int n = 0, i = 1, j = 1;
	COORD coord = { 0, 0 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	sick_list* ptr_list;
	system("cls");
	if (head_list == NULL)
	{
		printf("\n\nСписок записей пуст");
		return 0;
	}
	while (1)
	{
		SetConsoleCursorPosition(hConsole, coord);
		puts("..............................................Вывод данных........................................");
		printf("\n _________________________________________________________________________________________________"); 
		printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплата");
		printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|");
		printf("    Для перемещения по списку больничных листов\n                                                                                                          используйте клавиши \"Вверх\" и \"Вниз\"\n");
		printf("                                                                                                                                                           Для выбора листа нажмите Enter\n");
		printf("                                                                                                                                                           Для выхода в меню - esc\n");
		coord = { 0, 5 };
		SetConsoleCursorPosition(hConsole, coord);
		ptr_list = head_list;
		while (ptr_list != NULL)
		{
			if (j == i)
				SetConsoleTextAttribute(hConsole, 0x70);
			else
				SetConsoleTextAttribute(hConsole, 15);
			printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, ptr_list->num_of_list, ptr_list->Employee.name, ptr_list->Employee.secound_name, ptr_list->Employee.surname, ptr_list->year, ptr_list->mounth, ptr_list->days_count, ptr_list->Employee.salary_per_day, ptr_list->payment);
			printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
			SetConsoleTextAttribute(hConsole, 15);
			ptr_list = ptr_list->next_list;
			i++;
		}
		active_bottom = _getch();
		switch (active_bottom)
		{
		case 80:
			if (j == i - 1)
				j = 1;
			else
				j++;
			break;
		case 72:
			if (j == 1)
				j = i - 1;
			else
				j--;
			break;
		case 13:return j;
			break;
		case 27:return 0;
		}
		i = 1;
		coord = { 0, 0 };
	}
}
int wish_account()	//функция выбора нужного больничного листа из списка с помощью клавиш "Вверх" "Вниз"
{
	char active_bottom;
	int n = 0, i = 1, j = 1;
	COORD coord = { 0, 0 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	account* ptr_account;
	system("cls");
	if (head_account == NULL)
	{
		printf("\n\nСписок пользователей пуст");
		return 0;
	}
	while (1)
	{
		SetConsoleCursorPosition(hConsole, coord);
		puts("........................Вывод данных о пользователях............................");
		printf("\n ___________________________________________________________________");
		printf("\n|%5s|%20s|%20s|%20s|", "№", "Login", "Password", "Статус");
		printf("\n|_____|____________________|____________________|____________________|\n");
		coord = { 0, 5 };
		SetConsoleCursorPosition(hConsole, coord);
		ptr_account = head_account;
		while (ptr_account != NULL)
		{
			if (j == i)
				SetConsoleTextAttribute(hConsole, 0x70);
			else
				SetConsoleTextAttribute(hConsole, 15);
			printf("|%5d|%20s|%20s|%20d|", i, ptr_account->login, ptr_account->password, ptr_account->status);
			printf("\n|_____|____________________|____________________|____________________|\n");
			ptr_account = ptr_account->next_account;
			i++;
		}
		active_bottom = _getch();
		switch (active_bottom)
		{
		case 80:
			if (j == i - 1)
				j = 1;
			else
				j++;
			break;
		case 72:
			if (j == 1)
				j = i - 1;
			else
				j--;
			break;
		case 13:return j;
			break;
		case 27:return 0;
		}
		i = 1;
		coord = { 0, 0 };
	}
}
void GoToXY(short x, short y)
{
	SetConsoleCursorPosition(hStdOut, { x, y });
}
void ConsoleCursorVisible(bool show, short size)
{
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(hStdOut, &structCursorInfo);
	structCursorInfo.bVisible = show; // изменяем видимость курсора
	structCursorInfo.dwSize = size;   // изменяем размер курсора
	SetConsoleCursorInfo(hStdOut, &structCursorInfo);
}

int intread(int t) // Ввод числа типа int 
{
	char c, mas[10];
	int i = 0;
	while (1)
	{
		do
		{
			c = _getch();
		} while (!(c >= 48 && c <= 57) && c != '\r' && c != '\b');//ограничиваем ввод, допуская лишь цифры

		if (c != '\r' && c != '\b' && i != t)
		{
			mas[i] = c;
			i++;
			printf("%c", c);
		}
		else if (c == '\b' && i != 0)
		{
			printf("\b \b");
			i--;
		}
		else if (c == '\r' && i != 0)
			break;
	}
	mas[i] = '\0';

	i = atoi(mas);
	puts("\r");
	return i;
}
int yearread()
{
	int current_year = 2022;
	int temp_year;
	bool invalid_enter = false; //изначально ввод корректен
	do
	{
		if (invalid_enter == true) //при вторничном попадании на эту строку значение истинно, поэтому выскакивает подсказка
		{
			printf("Нельзя ввести год, которого ещё не было, либо раньше текущего более, чем на полвека!");
			printf("\nПопробуйте ещё раз: ");
		}

		temp_year = intread(4);
		invalid_enter = true;
	} while (temp_year > current_year || temp_year < current_year - 50);
	return temp_year;
}
int mounthread()
{
	int temp_mounth;
	bool invalid_enter = false; //идеальный случай - ввод корректен с первого раза, поэтому значение некорректного ввода ложно
	do
	{
		if (invalid_enter == true)
		{
			printf("Нельзя ввести несуществующий месяц!");
			printf("\nПопробуйте ещё раз: ");
		}
		temp_mounth = intread(2);
		invalid_enter = true;
	} while (temp_mounth > 12 || temp_mounth < 1);
	return temp_mounth;
}
int dayscountread(int mounth, int year)
{
	int temp_count;
	bool invalid_enter = false; //идеальный случай - ввод корректен с первого раза, поэтому значение некорректного ввода ложно
	switch (mounth)
	{
	case 2:
		if (year % 4 == 0)
		{
			do
			{
				if (invalid_enter == true)
				{
					printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
					printf("\nПопробуйте ещё раз: ");
				}
				temp_count = intread(2);
				invalid_enter = true;
			} while (temp_count > 29 || temp_count < 1);
		}
		else
		{
			do
			{
				if (invalid_enter == true)
				{
					printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
					printf("\nПопробуйте ещё раз: ");
				}
				temp_count = intread(2);
				invalid_enter = true;
			} while (temp_count > 28 || temp_count < 1);
		}
		break;
	case 4:
		do
		{
			if (invalid_enter == true)
			{
				printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
				printf("\nПопробуйте ещё раз: ");
			}
			temp_count = intread(2);
			invalid_enter = true;
		} while (temp_count > 30 || temp_count < 1);
		break;
	case 6:
		do
		{
			if (invalid_enter == true)
			{
				printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
				printf("\nПопробуйте ещё раз: ");
			}
			temp_count = intread(2);
			invalid_enter = true;
		} while (temp_count > 30 || temp_count < 1);
		break;
	case 9:
		do
		{
			if (invalid_enter == true)
			{
				printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
				printf("\nПопробуйте ещё раз: ");
			}
			temp_count = intread(2);
			invalid_enter = true;
		} while (temp_count > 30 || temp_count < 1);
		break;
	case 11:
		do
		{
			if (invalid_enter == true)
			{
				printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
				printf("\nПопробуйте ещё раз: ");
			}
			temp_count = intread(2);
			invalid_enter = true;
		} while (temp_count > 30 || temp_count < 1);
		break;
	default:
		do
		{
			if (invalid_enter == true)
			{
				printf("Нельзя ввести дней больше, чем в этом месяце вообще было!");
				printf("\nПопробуйте ещё раз: ");
			}
			temp_count = intread(2);
			invalid_enter = true;
		} while (temp_count > 31 || temp_count < 1);
	}
	return temp_count;
}

void print_users()		//вывод данных об учетных записях в табличной форме
{
	int i = 1;
	system("cls");
	if (head_account == NULL)
	{
		printf("\n\nСписок пользователей пуст!\n");
		return;
	}
	puts("........................Вывод данных о пользователях............................");
	printf("\n ___________________________________________________________________");
	printf("\n|%5s|%20s|%20s|%20s|", "№", "Login", "Password", "Статус");
	printf("\n|_____|____________________|____________________|____________________|\n");
	Account* ptr = head_account;
	while (ptr != NULL)
	{
		printf("|%5d|%20s|%20s|%20d|", i, ptr->login, ptr->password, ptr->status);
		printf("\n|_____|____________________|____________________|____________________|\n");
		ptr = ptr->next_account;
		i++;
	}
}
void print_list()			//вывод записей на экран в табличном виде
{
	int n = 0, i = 1;
	system("cls");
	puts("..............................................Вывод данных........................................");
	printf("\n _________________________________________________________________________________________________");
	printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
	printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
	sick_list* ptr_list = head_list;
	while (ptr_list != NULL)
	{
		printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, ptr_list->num_of_list, ptr_list->Employee.name, ptr_list->Employee.secound_name, ptr_list->Employee.surname, ptr_list->year, ptr_list->mounth, ptr_list->days_count, ptr_list->Employee.salary_per_day, ptr_list->payment);
		printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
		ptr_list = ptr_list->next_list;
		i++;
	}
}

int delete_first(sick_list** head) //удаление первого элемента списка
{
	sick_list* prev = NULL;
	if (head == NULL)
	{
		exit(-1);
	}
	prev = (*head);
	(*head) = (*head)->next_list;
	free(prev);
}
int delNlist(sick_list** head, int n) //удаление n-ого узла из списка
{
	if (n < 1 || n > list_count)
		return 0;
	if (n == 1) {
		delete_first(head);
		return 1;
	}
	else
	{
		sick_list* prev = getNlist(*head, n - 1);
		sick_list* temp_element = prev->next_list;
		prev->next_list = temp_element->next_list;
		free(temp_element);
	}
	return 2;
}
int delAccount(account** head, int n)
{
	if (n < 1 || n > account_count)
	{
		printf("Вы не можете удалить несуществующих пользователей");
		return 0;
	}
	if (n == 1)
	{
		printf("Вы не можете удалить всех пользователей");
		return 1;
	}
	else
	{
		account* prev = getNaccount(*head, n - 1);
		account* temp_element = prev->next_account;
		prev->next_account = temp_element->next_account;
		free(temp_element);
	}
	return 2;
}

void new_list()	//добавление нового больничного листа
{
	char temp_name[BUFSIZE], temp_secound_name[BUFSIZE], temp_surname[BUFSIZE];
	sick_list* tmp = new sick_list;
	puts("............................................Добавление записи................................................");
	getchar();
	printf("\nВведите имя:");
	cin.get(temp_name, BUFSIZE);
	strcpy_s(tmp->Employee.name, temp_name);
	getchar();
	printf("\nВведите фамилию:");
	cin.get(temp_secound_name, BUFSIZE);
	strcpy_s(tmp->Employee.secound_name, temp_secound_name);
	getchar();
	printf("\nВведите отчество:");
	cin.get(temp_surname, BUFSIZE);
	strcpy_s(tmp->Employee.surname, temp_surname);
	getchar();
	printf("\nВведите ставку:");
	tmp->Employee.salary_per_day = intread(5); //проверка на int
	printf("\nВведите номер больничного листа:");
	tmp->num_of_list = intread(5); //проверка на int
	printf("\nВведите год болезни:");
	tmp->year = yearread(); //проверка на год не больше данного, не позже 50 лет от данного
	printf("\nВведите месяц болезни:");
	tmp->mounth = mounthread(); // проверка на 12
	printf("\nВведите количество больничных дней:");
	tmp->days_count = dayscountread(tmp->mounth, tmp->year); //проверка на 28-30 дней
	tmp->payment = (tmp->Employee.salary_per_day) * (tmp->days_count); //подсчёты выплат
	tmp->next_list = head_list;
	head_list = tmp;
	list_count++;
	printf("\nЗапись успешно добавлена!\n");
	system("pause");
	system("CLS");
}
void new_user()
{
	account* tmp = new account;
	int flag = 0;
	char temp_login[BUFSIZE], temp_password[BUFSIZE];

	puts("..........................................Добавление аккаунта..............................................");
	getchar();
	printf("\nВведите логин:");
	cin.get(temp_login, BUFSIZE);
	strcpy_s(tmp->login, temp_login);

	account* p = head_account;
	
	while (p && flag == 0)
	{
		if (strstr(p->login, temp_login) != 0)
		{
			flag++;
		}
		p = p->next_account;
	}
	if (flag != 0)
		printf("\n\nТакой логин уже есть\nПопробуйте ввести другой\n");
	else 
	{
		getchar();
		printf("\nВведите пароль:");
		cin.get(temp_password, BUFSIZE);
		strcpy_s(tmp->password, temp_password);
		getchar();
		printf("\nВведите статус:");
		tmp->status = intread(1); //проверка на int

		tmp->next_account = head_account;
		head_account = tmp;
		account_count++;
		printf("\nАккаунт успешно добавлен!\n");
	}
	system("pause");
	system("CLS");
}

bool menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	GoToXY(x, y);
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0: 
	{
		login(); 
		break;
	}
	case 1: 
	{
		
		printf("Выбран выход из программы");
		break; 
	}
	}
	return active_menu;
	system("CLS");
}

bool edit_personal_info_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	sick_list* temp_to_edit = TEMP_LIST;
	char temp[BUFSIZE];
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		getchar();
		printf("Введите новую фамилию: ");
		cin.get(temp, BUFSIZE);
		strcpy_s(temp_to_edit->Employee.secound_name, temp);
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
		
	}
	case 1:
	{
		getchar();
		printf("Введите новое имя: ");
		cin.get(temp, BUFSIZE);
		strcpy_s(temp_to_edit->Employee.name, temp);
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 2:
	{
		getchar();
		printf("Введите новое отчество: ");
		cin.get(temp, BUFSIZE);
		strcpy_s(temp_to_edit->Employee.surname, temp);
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 3:
	{
		printf("Введите новую ставку: ");
		int tmp_spd = intread(3);
		temp_to_edit->Employee.salary_per_day = tmp_spd;
		temp_to_edit->payment = temp_to_edit->Employee.salary_per_day * temp_to_edit->days_count;
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 4: break;
	}
	return active_menu;
	return true;
	system("CLS");
}
bool edit_sick_list_data_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	sick_list* temp_to_edit = TEMP_LIST;
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		printf("Введите новый номер больничного листа: ");
		temp_to_edit->num_of_list = intread(10);
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 1:
	{
		printf("Введите другой год: ");
		temp_to_edit->year = yearread();
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 2:
	{
		printf("Введите другой месяц: ");
		temp_to_edit->mounth = mounthread();
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 3:
	{
		printf("Введите новое количество дней: ");
		temp_to_edit->days_count = intread(2);
		printf("\nРедактирование успешно завершено!\n");
		system("pause");
		break;
	}
	case 4: break;
	}
	/*_getch();*/
	return active_menu;
	return true;
	system("CLS");
}
bool edit_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		string personal_info_field_menu[] = { "Фамилия", "Имя", "Отчество", "Ставка", "Назад" };
		function(personal_info_field_menu, 4, edit_personal_info_menu);
		break;
	}
	case 1:
	{
		string sick_list_field_menu[] = { "Номер больничного листа", "Год болезни", "Месяц болезни", "Количество дней", "Назад" };
		function(sick_list_field_menu, 4, edit_sick_list_data_menu);
		break;
	}
	case 2:
		break;
	}
	/*_getch();*/
	return active_menu;
	return true;
	system("CLS");
}
void edit_status()
{
	account* temp_to_edit = head_account;
	system("cls");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("Какой статус пользователю хотите присвоить? ");
	temp_to_edit->status = intread(1);
	printf("\nРедактирование успешно завершено!\n");
	system("pause");
	system("CLS");
	return;
}
int editNlist(sick_list** head, int n) //редактирование n-ого узла из списка
{
	sick_list* prev = getNlist(*head, n - 1);
	sick_list* temp_element = prev->next_list;
	TEMP_LIST = temp_element;
	system("CLS");
	string edit_list_menu[] = { "Личная информация", "Данные больничного листа", "Вернуться" };
	function(edit_list_menu, 2, edit_menu);
	return 2;
}
int editNaccount(account** head, int n)
{
	account* prev = getNaccount(*head, n - 1);
	account* temp_element = prev->next_account;
	TEMP_ACCOUNT = temp_element;
	system("CLS");
	edit_status();
	return 2;
}

bool Admin_account_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		new_user();
		break;
	}
	case 1:
	{
		if (isEmpty(account_count) == 0)
		{
			z = wish_account();
			if (z != 0)
			{
				editNaccount(&head_account, z);
				system("pause");
			}
		}
		system("cls");
		break;
	}
	case 2:
	{
		if (isEmpty(account_count) == 0)
		{
			z = wish_account();
			if (z != 0)
			{
				delAccount(&head_account, z);
				account_count--;
				printf("Удаление успешно завершено!");
				system("pause");
			}
		}
		system("cls");
		break;
	}case 3:
	{
		if (isEmpty(account_count) == 0)
		{
			print_users();
		}
		system("pause");
		break;
	}
	case 4:
	{
		if (isEmpty(account_count) == 0)
		{
			string account_sort_menu[] = { "Логин", "Статус", "Назад" };
			function(account_sort_menu, 2, sort_account_menu);
		}
		system("pause");
		break;
	}
	case 5: break;
	}
	_getch();
	return active_menu;
	return true;
	system("CLS");
}
bool Admin_data_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		new_list();
		system("cls");
		break;
	}
	case 1:
	{
		if (isEmpty(list_count) == 0)
		{
			z = wish_sick_list();
			if (z != 0)
			{
				editNlist(&head_list, z);
				system("pause");
			}
		}
		system("cls");
		break;
	}
	case 2:
	{
		if (isEmpty(list_count) == 0)
		{
			z = wish_sick_list();
			if (z != 0)
			{
				delNlist(&head_list, z);
				list_count--;
				printf("Удаление успешно завершено!");
				system("pause");
			}
		}
		system("cls");
		break;
	}
	case 3:
	{
		if (isEmpty(list_count) == 0)
		{
			print_list();
		}
		system("pause");
		system("cls");
		break;
	}
	case 4:
	{
		if (isEmpty(list_count) == 0)
		{
			string Search_Menu[] = { "Номер листа", "Имя","Фамилия", "Год болезни", "Месяц болезни", "Кол-во дней", "Дневная ставка", "Итоговые выплаты","Назад" };
			function(Search_Menu, 8, search_menu);
		}
		system("cls");
		break;
	}
	case 5:
	{
		if (isEmpty(list_count) == 0)
		{
			string sick_list_sort_menu[] = { "Номер больничного листа", "Фамилия", "Имя", "Ставка", "Год болезни", "Месяц болезни", "Количество дней", "Итоговые выплаты", "Назад" };
			function(sick_list_sort_menu, 8, sort_data_menu);
		}
		system("pause");
		system("cls");
		break;
	}
	case 6: { individual_task(); break; }
	case 7: { break; }
	}
	_getch();
	return active_menu;
	return true;
	system("CLS");
}
bool Admin_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		list_from_file();
		string ADMIN_data_menu[] = { "Добавление сведений о больничном листе", "Редактирование больничного листа", "Удаление больничного листа", "Просмотр всех данных в табличной форме", "Поиск по параметру", "Сортировка данных", "Индивидуальное задание", "Возвращение в меню администратора" };
		function(ADMIN_data_menu, 7, Admin_data_menu);
		break;
	}
	case 1:
	{
		string ADMIN_account_menu[] = { "Создание учётной записи", "Редактирование статуса аккаунтов", "Удаление учётной записи", "Просмотр учётных записей", "Сортировка данных", "Возвращение в меню администратора" };
		function(ADMIN_account_menu, 5, Admin_account_menu);
		break;
	}
	case 2:
		break;
	}
	list_to_file();
	return active_menu;
	return true;
	system("CLS");
}
bool User_data_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	list_from_file();
	switch (active_menu)
	{
	case 0:
	{
		print_list();
		system("pause");
		system("cls");
		break;
	}
	case 1:
	{
		if (isEmpty(list_count) == 0)
		{
			string Search_Menu[] = { "Номер листа", "Имя","Фамилия", "Год болезни", "Месяц болезни", "Кол-во дней", "Дневная ставка", "Итоговые выплаты","Назад" };
			function(Search_Menu, 8, search_menu);
		}
		system("pause");
		system("cls");
		break;
	}
	case 2:
	{
		if (isEmpty(list_count) == 0)
		{
			string sick_list_sort_menu[] = { "Номер больничного листа", "Фамилия", "Имя", "Ставка", "Год болезни", "Месяц болезни", "Количество дней", "Итоговые выплаты", "Назад" };
			function(sick_list_sort_menu, 8, sort_data_menu);
		}
		system("pause");
		system("cls");
		break;
	} 
	case 3: { individual_task(); break; }
	case 4: { break; }
	}
	return active_menu;
	return true;
	system("CLS");
}

void individual_task()
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	isEmpty(list_count);
	int i = 1, flag0 = 0;
	int buf, buff;
	int payment_sum = 0;
	sick_list* p = head_list;
	printf("\nИскать записи по X месяцу Y года:");
	printf("\nX:");
	buf = mounthread();
	printf("\nY:");
	buff = yearread();
	while (p)
	{
		if ((p->mounth == buf) && (p->year == buff))
		{
			if (flag0 == 0)
			{
				printf("\n....................................Результаты поиска........................................\n");
				printf("\n _________________________________________________________________________________________________");
				printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
			}
			printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
			printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
			flag0++;
			payment_sum += (p->payment);
		}
		p = p->next_list;
	}
	if (flag0 == 0)
		printf("\n\nСовпадений не найдено!");
	else
	{
		printf("\n                                                                                           \n");
		printf("\n...........................................................................................\n");
		printf("Итоговая сумма выплат по больничным листам за %d месяц %d года составила %d", buf, buff, payment_sum);
	}
	printf("\n\n");
	system("pause");
	system("CLS");
}
bool search_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	isEmpty(list_count);
	int i = 1, flag0 = 0;
	int buf, buff;
	char buffer[BUFSIZE], buffer1[BUFSIZE];
	int f = 0;
	sick_list* p = head_list;
	switch (active_menu)
	{
	case 0:
	{
		system("cls");
		printf("\nИскать следующий номер листа:");
		buf = intread(6);
		while (p)
		{
			if (p->num_of_list == buf)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 1:
	{
		system("cls");
		printf("\nИскать по следующему имени:");
		cin.get(buffer, BUFSIZE);
		cout << "Вы ввели строку: " << buffer << endl;
		strcpy_s(buffer1, buffer);
		buffer1[0] = buffer1[0] + 32;
		while (p)
		{
			if (strstr(p->Employee.name, buffer) != 0 || strstr(p->Employee.name, buffer1) != 0)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
					flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 2:
	{
		system("cls");
		printf("\nИскать следующую фамилию:");
		cin.get(buffer, BUFSIZE);
		strcpy_s(buffer1, buffer);
		buffer1[0] = buffer1[0] + 32;
		while (p)
		{
			if (strstr(p->Employee.secound_name, buffer) != 0 || strstr(p->Employee.name, buffer1) != 0)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 3:
	{
		system("cls");
		printf("\nИскать записи по следующему году:");
		buf = yearread();
		while (p)
		{
			if (p->year == buf)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 4:
	{
		system("cls");
		printf("\nИскать записи по X месяцу Y года:");
		printf("\nX:");
		buf = mounthread();
		printf("\nY:");
		buff = yearread();
		while (p)
		{
			if ((p->mounth == buf) && (p->year == buff))
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 5:
	{
		system("cls");
		printf("\nИскать записи по следующему количеству дней:");
		buf = intread(2);
		while (p)
		{
			if (p->days_count == buf)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 6:
	{
		system("cls");
		printf("\nИскать следующую ставку:");
		buf = intread(2);
		while (p)
		{
			if (p->Employee.salary_per_day == buf)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 7:
	{
		system("cls");
		printf("\nИскать записи по следующим выплатам:");
		buf = intread(5);
		while (p)
		{
			if (p->payment == buf)
			{
				if (flag0 == 0)
				{
					printf("\n....................................Результаты поиска........................................\n");
					printf("\n _________________________________________________________________________________________________");
					printf("\n|%3s|%10s|%10s|%10s|%10s|%7s|%7s|%13s|%7s|%10s|", "№", "Номер листа", "Имя", "Фамилия", "Отчество", "Год", "Месяц", "Кол-во дней", "Ставка", "Выплаты");
					printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				}
				printf("|%3d|%11d|%10s|%10s|%10s|%7d|%7d|%13d|%7d|%10d|", i, p->num_of_list, p->Employee.name, p->Employee.secound_name, p->Employee.surname, p->year, p->mounth, p->days_count, p->Employee.salary_per_day, p->payment);
				printf("\n|___|___________|__________|__________|__________|_______|_______|_____________|_______|__________|\n");
				flag0++;
			}
			p = p->next_list;
		}
		if (flag0 == 0)
			printf("\n\nСовпадений не найдено!");
		printf("\n\n");
		system("pause");
		system("cls");
		break;
	}
	case 8:break;
	}
	return active_menu;
	return true;
	system("CLS");
}

void sort_login()
{
	account* temp1;
	account* temp2;
	account* prev;

	prev = head_account;
	temp2 = head_account;
	temp1 = head_account;
	temp2 = temp2->next_account;

	for (int i = 1; i < account_count; i++)
	{
		prev = head_account;
		temp2 = head_account;
		temp1 = head_account;
		temp2 = temp2->next_account;

		for (int j = 0; j < account_count - i; j++)
		{
			if (temp1->login > temp2->login)
			{
				if (temp1 == head_account)
					head_account = temp2;

				temp1->next_account = temp2->next_account;
				temp2->next_account = temp1;
				if (j > 0)
				{
					prev->next_account = temp2;
					prev = prev->next_account;
				}
				else
				{
					prev = head_account;
				}

				temp1 = temp2->next_account;
				temp2 = temp1->next_account;
			}
			else
			{
				if (j > 0)
				{
					prev = prev->next_account;
				}
				temp1 = temp1->next_account;
				temp2 = temp2->next_account;
			}
		}
	}
}
void sort_status()
{
	account* temp1;
	account* temp2;
	account* prev;

	prev = head_account;
	temp2 = head_account;
	temp1 = head_account;
	temp2 = temp2->next_account;

	for (int i = 1; i < account_count; i++)
	{
		prev = head_account;
		temp2 = head_account;
		temp1 = head_account;
		temp2 = temp2->next_account;

		for (int j = 0; j < account_count - i; j++)
		{
			if (temp1->status > temp2->status)
			{
				if (temp1 == head_account)
					head_account = temp2;

				temp1->next_account = temp2->next_account;
				temp2->next_account = temp1;
				if (j > 0)
				{
					prev->next_account = temp2;
					prev = prev->next_account;
				}
				else
				{
					prev = head_account;
				}

				temp1 = temp2->next_account;
				temp2 = temp1->next_account;
			}
			else
			{
				if (j > 0)
				{
					prev = prev->next_account;
				}
				temp1 = temp1->next_account;
				temp2 = temp2->next_account;
			}

		}
	}
}
bool sort_account_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		if (isEmpty(account_count) == 0)
		{
			sort_login(); 
			print_users(); 
			system("pause");
			system("cls");
		}
		break;
	}
	case 1:
	{
		if (isEmpty(account_count) == 0) 
		{
			sort_status(); 
			print_users(); 
			system("pause"); 
			system("cls");
		}
	break;
	}
	case 2: break;
	}
	return active_menu;
	return true;
	system("CLS");
}
void sort_num_of_list()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->num_of_list > temp2->num_of_list)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_year()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->year > temp2->year)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_mounth()
{
	sort_year();
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->mounth > temp2->mounth)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_days_count()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->days_count > temp2->days_count)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_name()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->Employee.name > temp2->Employee.name)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_secound_name()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->Employee.secound_name > temp2->Employee.secound_name)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_salary_per_day()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->Employee.salary_per_day > temp2->Employee.salary_per_day)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
void sort_payment()
{
	sick_list* temp1;
	sick_list* temp2;
	sick_list* prev;

	prev = head_list;
	temp2 = head_list;
	temp1 = head_list;
	temp2 = temp2->next_list;

	for (int i = 1; i < list_count; i++)
	{
		prev = head_list;
		temp2 = head_list;
		temp1 = head_list;
		temp2 = temp2->next_list;

		for (int j = 0; j < list_count - i; j++)
		{
			if (temp1->payment > temp2->payment)
			{
				if (temp1 == head_list) head_list = temp2;

				temp1->next_list = temp2->next_list;
				temp2->next_list = temp1;

				if (j > 0)
				{
					prev->next_list = temp2;
					prev = prev->next_list;
				}
				else prev = head_list;

				temp1 = temp2->next_list;
				temp2 = temp1->next_list;
			}
			else
			{
				if (j > 0) prev = prev->next_list;
				temp1 = temp1->next_list;
				temp2 = temp2->next_list;
			}
		}
	}
}
bool sort_data_menu(int x, int y, int active_menu, bool exit)
{
	system("CLS");
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	switch (active_menu)
	{
	case 0:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_num_of_list();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 1:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_name();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 2:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_secound_name();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 3:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_salary_per_day();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 4:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_year();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 5:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_mounth();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 6:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_days_count();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 7:
	{
		if (isEmpty(list_count) == 0)
		{
			sort_payment();
			print_list();
			system("pause");
			system("CLS");
		}
		break;
	}
	case 8: { printf("Выбран выход из программы"); break; }
	}
	_getch();
	return active_menu;
	return true;
	system("CLS");
}

void secretpass(char mas[BUFSIZE]) //функция ввода пароля, контролирующая ввод и заменяющая символы на экране на '*'
{
	char c;
	int i = 0;
	while (1)
	{
		do
		{
			c = _getch();
		} while (!(c >= 65 && c <= 90) && !(c >= 97 && c <= 122) && !(c >= 48 && c <= 57) && c != '\r' && c != '\b');

		if (c != '\r' && c != '\b' && i != 20)
		{
			mas[i] = c;
			i++;
			printf("%c", '*');//Вывод на экран символа '*'
		}
		else if (c == '\b' && i != 0)
		{
			printf("\b \b");
			i--;
		}
		else if (c == '\r' && i != 0)
			break;
	}
	mas[i] = '\0';
	puts("\r");
}
int encryption(char code[])   //функция шифрования пароля
{
	//шифрование гаммированием (замена кода с использованием константы key)
	int i = 0, j;
	for (j = 0; code[j] != '\0'; j++)
	{
		code[j] = (code[j] + key[i]) % 128;
		if (i < strlen(key) - 1)
			i++;
		else
			i = 0;
	}
	return 1;
}
int decryption(char cipher[])     //функция дешифровки пароля
{
	int i = 0, j;
	for (j = 0; cipher[j] != '\0'; j++)
	{
		cipher[j] = (cipher[j] - key[i] + 128) % 128;
		if (i < strlen(key) - 1)
			i++;
		else
			i = 0;
	}
	return 1;
}

bool function(string* menu_arr, int menu_lenth, bool (*func)(int x, int y, int active_menu, bool exit)) //общая функция, которая позволяет оформить интерактивное меню для различных случаев
{
	int active_menu = 0;
	char num_code;
	bool exit = true;

	while (exit)
	{
		int x = 35, y = 10;
		GoToXY(x, y);

		bool (*funck_name)(int x, int y, int active_menu, bool exit);
		funck_name = func;

		for (int i = 0; i < menu_lenth + 1; i++)
		{
			if (i == active_menu) 	SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | COMMON_LVB_UNDERSCORE);
			else SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			GoToXY(x, y++);
			cout << *(menu_arr + i) << endl;

		}
		num_code = _getch();
		if (num_code == -32) num_code = _getch();
		switch (num_code)
		{
		case 72:
			if (active_menu > 0)
				--active_menu;
			break;
		case 80:
			if (active_menu < size(*menu_arr) - 1)
				++active_menu;
			break;
		case 13:
			funck_name(x, y, active_menu, exit);
			if (active_menu == menu_lenth) return exit;
		}
	}
	SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void login()		//авторизация
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	int flag0 = 0;
	char temp_login[BUFSIZE], temp_pass[BUFSIZE];
	system("cls"); 
	getchar();
	account* p = head_account;
	COORD coord = { 10, 2 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	printf(" _____________________________\n");
	printf("|Логин   |                    |\n");
	printf("|________|____________________|\n");
	printf("|Пароль  |                    |\n");
	printf("|________|____________________|\n");
	coord.Y = 2;
	SetConsoleCursorPosition(hConsole, coord);
	cin.get(temp_login, BUFSIZE);
	coord.Y = 4;
	SetConsoleCursorPosition(hConsole, coord);
	secretpass(temp_pass);
	
	while (p && flag0 == 0)
	{
		if (strstr(p->login, temp_login) != 0 && strstr(p->password, temp_pass) != 0)
		{
			flag0++;
		}
		if (flag0 == 0) p = p->next_account;
	}
	if (flag0 == 0)
		printf("\n\nПроверьте корректность введенного логина и пароля!");
	else
	{
	printf("\nВход осуществлен!\n");
	if (p->status == 0)
	{
		string USER_data_menu[] = { "Просмотр всех больничных листов", "Поиск по параметру", "Сортировка больничных листов", "Индивидуальное задание", "Возвращение в меню пользователя" };
		function(USER_data_menu, 4, User_data_menu);
	}
	if (p->status == 1)
	{
		string ADMIN_menu[] = { "Работа с данными больничных листов", "Работа с учётными записями", "Выход из меню администратора" };
		function(ADMIN_menu, 2, Admin_menu);
	}
	printf("\n\n");
	system("pause");
	system("cls");
	}
}
sick_list* list_from_file()
{
	sick_list *prev;
	ifstream Inform("Sick_list.txt");

	if (Inform.is_open())
	{
		Inform.seekg(0, ios_base::end);
		if (Inform.tellg() == 0)
		{
			Inform.close();
			return NULL;
		}
		Inform.seekg(0, ios_base::beg);

		head_list = new sick_list;

		Inform >> head_list->num_of_list;
		Inform >> head_list->Employee.name;
		Inform >> head_list->Employee.secound_name;
		Inform >> head_list->Employee.surname;
		Inform >> head_list->year;
		Inform >> head_list->mounth;
		Inform >> head_list->days_count;
		Inform >> head_list->Employee.salary_per_day;
		Inform >> head_list->payment;

		head_list->next_list = NULL;
		list_count++;
		prev = head_list;
		if (Inform.peek() != EOF)
		{
			while (!Inform.eof())
			{
				sick_list* newP = new sick_list;

				Inform >> newP->num_of_list;
				Inform >> newP->Employee.name;
				Inform >> newP->Employee.secound_name;
				Inform >> newP->Employee.surname;
				Inform >> newP->year;
				Inform >> newP->mounth;
				Inform >> newP->days_count;
				Inform >> newP->Employee.salary_per_day;
				Inform >> newP->payment;

				newP->next_list = NULL;
				prev->next_list = newP;
				prev = newP;
				list_count++;
			}
		}
	}
	Inform.close();
	return head_list;
}
void list_to_file()
{
	sick_list *temp = head_list;
	ofstream Inform("Sick_list.txt", ios_base::out);
	if (Inform.is_open())
	{
		while (temp != NULL)
		{
			Inform << temp->num_of_list;
			Inform << temp->Employee.name << ' ';
			Inform << temp->Employee.secound_name << ' ';
			Inform << temp->Employee.surname << ' ';
			Inform << temp->year << ' ';
			Inform << temp->mounth << ' ';
			Inform << temp->days_count << ' ';
			
			Inform << temp->Employee.salary_per_day << ' ';
			Inform << temp->payment << ' ';
			
			if (temp->next_list != NULL)
				Inform << '\n';
			temp = temp->next_list;
		}
	}
	else printf("\nФайл не октрыт");
	Inform.close();
}
account* account_from_file()
{
	account* prev;
	ifstream AccInform("account.txt", ios_base::in);

	if (AccInform.is_open())
	{
		AccInform.seekg(0, ios_base::end);
		if (AccInform.tellg() == 0)
		{
			AccInform.close();
			return NULL;
		}
		AccInform.seekg(0, ios_base::beg);

		head_account = new account;

		AccInform >> head_account->login;
		AccInform >> head_account->password;
		decryption(head_account->password);
		AccInform >> head_account->status;
		head_account->next_account = NULL;
		account_count++;
		prev = head_account;

		if (AccInform.peek() != EOF)
		{
			while (!AccInform.eof())
			{
				account* newP = new account;
				AccInform >> newP->login;
				AccInform >> newP->password;
				decryption(newP->password);
				AccInform >> newP->status;

				newP->next_account = NULL;
				prev->next_account = newP;
				prev = newP;
				account_count++;
			}
		}
		AccInform.close();
		return head_account;
	}
}
void account_to_file()
{
	account* temp = head_account;
	ofstream AccInform("account.txt", ios_base::out);
	if (AccInform.is_open())
	{
		while (temp != NULL)
		{
			AccInform << temp->login << ' ';

			/*AccInform << ' ';*/
			encryption(temp->password);
			AccInform << temp->password;
			decryption(temp->password);
			AccInform << ' ' << temp->status << ' ';

			/*AccInform << temp->password << ' ';
			AccInform << temp->status << ' ';*/

			if (temp->next_account != NULL)
				AccInform << '\n';
			temp = temp->next_account;	
		}
	}
	else printf("\nФайл не октрыт");
	AccInform.close();
}