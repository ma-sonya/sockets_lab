#include <stdio.h>
#include<iostream>
#include "winsock2.h"
#include<string>
#include<ctime>
#include <fstream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

using namespace std;

const string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

string convertFiletoString(string& nameOfFile) {
	string msg1;
	ifstream file(nameOfFile);   // відкриваємо файл
	if (file.is_open() == true) {
		cout << "File is open!" << endl;
	}
	else {
		cout << "File is not open!" << endl;
	}


	char sym;
	char lastSym = '\0';
	while (!file.eof()) // поки файл не порожній, зчитуємо з файлу по одному символу і вводимо значення змінної
	{
		file >> sym;
		if (sym == '\n') {
			msg1.push_back(' ');
			continue;
		}
		else if ((lastSym == ' ') && (sym == lastSym)) {
			continue;
		}
		msg1.push_back(sym);
		lastSym = sym;
	}
	file.close();
	return msg1;
}



void fSend(const SOCKET& Connection, string& msg1, string& nameOfFile, ofstream& log) {

	msg1 = convertFiletoString(nameOfFile);
	int msg_size = msg1.size();
	send(Connection, (char*)&msg_size, sizeof(int), NULL);	//надсилаємо size
	send(Connection, msg1.c_str(), msg_size, NULL);			//надсилаємо message

	string logPr = "Sent: " + msg1 + " at: " + currentDateTime(); // записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";
}

void Send(const SOCKET& Connection, string& msg1, ofstream& log) {

	int msg_size = msg1.size();
	send(Connection, (char*)&msg_size, sizeof(int), NULL);	//надсилаємо size
	send(Connection, msg1.c_str(), msg_size, NULL);			//надсилаємо message

	string logPr = "Sent: " + msg1 + " at: " + currentDateTime(); // записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";
}

void Recv(const SOCKET& Connection, string& msg1, ofstream& log) {
	int msg_size = msg1.size();
	recv(Connection, (char*)&msg_size, sizeof(int), NULL);//Приймаємо розмір масиву
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	if (recv(Connection, msg, msg_size, NULL) >= 0) {//Приймаємо повідомлення
		string str(msg);
		msg1 = str;

		cout << "You get message!\nShow it?  //\"+\" if yes, \"-\" if no" << endl; //показати повідомлення?
		char a = '-';
		cin >> a;
		switch (a) {
		case '+':
			cout << msg1;
			break;
		case '-':
			break;
		}
	}

	string logPr = "Recieved: " + msg1 + " at: " + currentDateTime();// записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";

	delete[] msg;
}



int main()
{

	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("Error at WSAStartup()\n");
	}


	SOCKADDR_IN clientService;
	int sizeoffCS = sizeof(clientService);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(1044);					//1025+19

	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
	}

	if (connect(ConnectSocket, (SOCKADDR*)&clientService, *(&sizeoffCS)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		WSACleanup();
	}
	else {
		printf("Connected to server!\n");
	}

	//Надсилаємо повідомлення

	ofstream log("ClientLog.txt");//you should create "ClientLog.txt"
	if (log.is_open() == true) {
		cout << "Log is open!" << endl;
	}
	else {
		cout << "Log is not open!" << endl;
	}


	ifstream file1("1C.txt");
	ifstream file2("2C.txt");

	string name1 = "1C.txt";
	string text1 = convertFiletoString(name1);

	string name2 = "2C.txt";
	string text2 = convertFiletoString(name2);

	string msg;
	Recv(ConnectSocket, msg, log);
	while (1) {
		//getline(cin, msg, '\n');
		cin >> msg;
		if (msg == "Who") {
			Send(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
		}
		else if (msg == "Break") {
			Send(ConnectSocket, msg, log);
			break;
		}
		else if (msg == text1) {
			string n = "1C.txt";
			fSend(ConnectSocket, msg, text1, log);
			Recv(ConnectSocket, msg, log);
		}
		else if (msg == text2) {
			string n = "2C.txt";
			fSend(ConnectSocket, msg, text2, log);
			Recv(ConnectSocket, msg, log);
		}
		else {
			cout << "Wrong command. Try again" << endl;
		}

	}
	Recv(ConnectSocket, msg, log);
	log.close();
	if (log.is_open() != true) {
		cout << "Log is closed!" << endl;
	}
	else {
		cout << "Log is not closed!" << endl;
	}
	system("pause");

	closesocket(ConnectSocket);
	WSACleanup();
	system("pause");
	return 0;
}








/*
#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include<ctime>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;

const string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

void Recv(const SOCKET& ConnectSocket, string& msg_str, ofstream& log) {
	int MSGsize = msg_str.size();
	recv(ConnectSocket, (char*)&MSGsize, sizeof(int), NULL); //Приймаємо розмір масиву
	char* msg = new char[MSGsize + 1];
	msg[MSGsize] = '\0';
	recv(ConnectSocket, msg, MSGsize, NULL); //Приймаємо повідомлення
	string str(msg);
	msg_str = str;
	cout << msg;
	string logPr = "Recieved: " + msg_str + " at: " + currentDateTime();
	log << "[" << logPr.size() << "][" << logPr << "]\n";
	if (msg_str != "Symbols can be entered:") cout << endl;
	delete[] msg;
}

void Send(const SOCKET& ConnectSocket, string& msg_str, ofstream& log) { //TODO
	//while(msg.size() < 256)
	int MSGsize = msg_str.size();
	send(ConnectSocket, (char*)&MSGsize, sizeof(int), NULL);
	//cout << "sended size" << endl;
	send(ConnectSocket, msg_str.c_str(), MSGsize, NULL);
	//cout << "sended msg " << msg_str << endl;
	string logPr = "Sent: " + msg_str + " at: " + currentDateTime();
	log << "[" << logPr.size() << "][" << logPr << "]\n";
}


//Функція очистки та закриття програми
void CleanUpAndClose(SOCKET s) {
	closesocket(s);
	WSACleanup();
	system("pause");
	exit(0);
}

int main(int argc, char* argv[]) {

	//WSAStartup - функція для завантаження біблеотки
	WSAData WSA_Socket; //вказівник на стурктуру з деякою інформацією
	WORD DLLVersion = MAKEWORD(2, 2); // Робиму змінну в якій за допомогую функції буде запис числа версії бібліотеки в 16-ковій системі
	if (WSAStartup(DLLVersion, &WSA_Socket) != 0) { // а що в WSA_Socket?
		cout << "Error at WSAStartup()" << endl;
		system("pause");
		return 0;
	}
	else {
		cout << "No Error at WSAStartup()" << endl;
	}

	//Дані з якими треба буде зв'язати майбутній сокет
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1050); //1025 + 25(Варіант)

	//Під'єднуємося до сервера
	SOCKET ConnectSocket = socket(PF_INET, SOCK_STREAM, NULL);
	if (connect(ConnectSocket, (SOCKADDR*)&addr, *(&sizeofaddr)) == SOCKET_ERROR) {
		cout << "Failed to connect" << endl;
		CleanUpAndClose(ConnectSocket);
		system("pause");
	}
	cout << "Connected to server." << endl;

	//Надсилаємо повідомлення
	ofstream log("ClientLog.txt");
	if (log.is_open() == true) {
		cout << "Log is open!" << endl;
	}
	else {
		cout << "Log is not open!" << endl;
	}

	string msg;
	Recv(ConnectSocket, msg, log);
	while (1) {
		getline(cin, msg, '\n');
		if (msg == "Who") {
			Send(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
		}
		else if (msg == "Send") {
			Send(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
			int n = stoi(msg);
			char c;
			for (int i = 0; i < n; i++) {
				cin >> c;
				msg = c;
				Send(ConnectSocket, msg, log);
			}
			cout << "All symbols are entered. Here is the result: ";
			break;
		}
		else if (msg == "Break") {
			Send(ConnectSocket, msg, log);
			break;
		}
		else {
			cout << "WRONG COMMAND" << endl;
		}
	}
	Recv(ConnectSocket, msg, log);
	log.close();
	if (log.is_open() != true) {
		cout << "Log is closed!" << endl;
	}
	else {
		cout << "Log is not closed!" << endl;
	}
	system("pause");
	return 0;
}*/





/*
#include <stdio.h>
#include<iostream>
#include "winsock2.h"
#include<string>
#include<ctime>
#include <fstream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

using namespace std;

const string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

string convertFiletoString(string& nameOfFile) {
	string msg1;
	ifstream file(nameOfFile);   // открываем файл

	char sym;
	char lastSym = '\0';
	while (!file.eof()) // пока файл не пуст, считываем из файла по одному символу и вводим значение в переменную
	{
		file >> sym;
		if (sym == '\n') {
			msg1.push_back(' ');
			continue;
		}
		else if ((lastSym == ' ') && (sym == lastSym)) {
			continue;
		}
		msg1.push_back(sym);
		lastSym = sym;
	}
	file.close();
	return msg1;
}



void fSend(const SOCKET& Connection, string& msg1, string& nameOfFile, ofstream& log) {

	msg1 = convertFiletoString(nameOfFile);
	int msg_size = msg1.size();
	//	Packet packettype = P_ChatMessage;
	//	send(Connection, (char*)&packettype, sizeof(Packet), NULL);
	send(Connection, (char*)&msg_size, sizeof(int), NULL);	//sending size
	send(Connection, msg1.c_str(), msg_size, NULL);			//sending message

	string logPr = "Sent: " + msg1 + " at: " + currentDateTime(); // записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";
}

void Send(const SOCKET& Connection, string& msg1, ofstream& log) {

	int msg_size = msg1.size();
	//	Packet packettype = P_ChatMessage;
	//	send(Connection, (char*)&packettype, sizeof(Packet), NULL);
	send(Connection, (char*)&msg_size, sizeof(int), NULL);	//sending size
	send(Connection, msg1.c_str(), msg_size, NULL);			//sending message

	string logPr = "Sent: " + msg1 + " at: " + currentDateTime(); // записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";
}

void Recv(const SOCKET& Connection, string& msg1, ofstream& log) {
	int msg_size = msg1.size();
	recv(Connection, (char*)&msg_size, sizeof(int), NULL);//Приймаємо розмір масиву
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	if (recv(Connection, msg, msg_size, NULL) >= 0) {//Приймаємо повідомлення
		string str(msg);
		msg1 = str;

		cout << "You get message!\nShow it?  //\"+\" if yes, \"-\" if no" << endl; //показати повідомлення?
		char a = '-';
		cin >> a;
		switch (a) {
		case '+':
			cout << msg1;
			break;
		case '-':
			break;
		}
	}

	string logPr = "Recieved: " + msg1 + " at: " + currentDateTime();// записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";

	delete[] msg;
}



int main()
{

	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("Error at WSAStartup()\n");
	}


	SOCKADDR_IN clientService;
	int sizeoffCS = sizeof(clientService);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(1044);					//1025+19

	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
	}

	if (connect(ConnectSocket, (SOCKADDR*)&clientService, *(&sizeoffCS)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		WSACleanup();
	}
	else {
		printf("Connected to server!\n");
	}

	//Надсилаємо повідомлення

	ofstream log("ClientLog.txt");//you should create "ClientLog.txt"
	if (log.is_open() == true) {
		cout << "Log is open!" << endl;
	}
	else {
		cout << "Log is not open!" << endl;
	}

	string msg;
	Recv(ConnectSocket, msg, log);





	closesocket(ConnectSocket);
	WSACleanup();
	system("pause");
	return 0;
}


/*#include <stdio.h>
#include<iostream>
#include "winsock2.h"
#include<string>
#include<ctime>
#include <fstream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

using namespace std;

const string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

string convertFiletoString(string& nameOfFile) {
	string msg1;
	ifstream file(nameOfFile);   // открываем файл

	char sym;
	char lastSym = '\0';
	while (!file.eof()) // пока файл не пуст, считываем из файла по одному символу и вводим значение в переменную
	{
		file >> sym;
		if (sym == '\n') {
			msg1.push_back(' ');
			continue;
		}
		else if ((lastSym == ' ') && (sym == lastSym)) {
			continue;
		}
		msg1.push_back(sym);
		lastSym = sym;
	}
	file.close();
	return msg1;
}



void fSend(const SOCKET& Connection, string& msg1, string& nameOfFile, ofstream& log) {

	msg1 = convertFiletoString(nameOfFile);
	int msg_size = msg1.size();
	//	Packet packettype = P_ChatMessage;
	//	send(Connection, (char*)&packettype, sizeof(Packet), NULL);
	send(Connection, (char*)&msg_size, sizeof(int), NULL);	//sending size
	send(Connection, msg1.c_str(), msg_size, NULL);			//sending message

	string logPr = "Sent: " + msg1 + " at: " + currentDateTime(); // записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";
}

void Send(const SOCKET& Connection, string& msg1, ofstream& log) {

	int msg_size = msg1.size();
	//	Packet packettype = P_ChatMessage;
	//	send(Connection, (char*)&packettype, sizeof(Packet), NULL);
	send(Connection, (char*)&msg_size, sizeof(int), NULL);	//sending size
	send(Connection, msg1.c_str(), msg_size, NULL);			//sending message

	string logPr = "Sent: " + msg1 + " at: " + currentDateTime(); // записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";
}

void Recv(const SOCKET& Connection, string& msg1, ofstream& log) {
	int msg_size = msg1.size();
	recv(Connection, (char*)&msg_size, sizeof(int), NULL);//Приймаємо розмір масиву
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	if (recv(Connection, msg, msg_size, NULL) >= 0) {//Приймаємо повідомлення
		string str(msg);
		msg1 = str;

		cout << "You get message!\nShow it?  //\"+\" if yes, \"-\" if no" << endl; //показати повідомлення?
		char a = '-';
		cin >> a;
		switch (a) {
		case '+':
			cout << msg1;
			break;
		case '-':
			break;
		}
	}

	string logPr = "Recieved: " + msg1 + " at: " + currentDateTime();// записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";

	delete[] msg;
}



int main()
{
	ifstream file1("1C.txt");
	ifstream file2("2C.txt");

	string name1 = "1C.txt";
	string text1 = convertFiletoString(name1);

	string name2= "2C.txt";
	string text2 = convertFiletoString(name2);


	WSADATA wsaDATA;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
		printf("Error at WSAStartup()\n");
	}


	SOCKADDR_IN clientService;
	int sizeoffCS = sizeof(clientService);
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
	clientService.sin_port = htons(1044);					//1025+19

	SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
	}

	if (connect(ConnectSocket, (SOCKADDR*)&clientService, *(&sizeoffCS)) == SOCKET_ERROR) {
		printf("Failed to connect.\n");
		WSACleanup();
	}
	else {
		printf("Connected to server!\n");
	}

	//Надсилаємо повідомлення

	ofstream log("ClientLog.txt");//you should create "ClientLog.txt"
	if (log.is_open() == true) {
		cout << "Log is open!" << endl;
	}
	else {
		cout << "Log is not open!" << endl;
	}

	string msg;
	Recv(ConnectSocket, msg, log);
	while (1) {
		getline(cin, msg, '\n');
		if (msg == "Who") {
			Send(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
		}
		else if (msg == "Break") {
			Send(ConnectSocket, msg, log);
			break;
		}
		else if (msg == text1) {
			string n = "1C.txt";
			fSend(ConnectSocket, msg, text1, log);
			Recv(ConnectSocket, msg, log);
		}
		else if (msg == text2) {
			string n = "2C.txt";
			fSend(ConnectSocket, msg, text2, log);
			Recv(ConnectSocket, msg, log);
		}
		else {
			cout << "Wrong command. Try again" << endl;
		}

	}





	closesocket(ConnectSocket);
	WSACleanup();
	system("pause");
	return 0;
}



/*
#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include<ctime>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;

const string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

void Recv(const SOCKET& ConnectSocket, string& msg_str, ofstream& log) {
	int MSGsize = msg_str.size();
	recv(ConnectSocket, (char*)&MSGsize, sizeof(int), NULL); //Приймаємо розмір масиву
	char* msg = new char[MSGsize + 1];
	msg[MSGsize] = '\0';
	recv(ConnectSocket, msg, MSGsize, NULL); //Приймаємо повідомлення
	string str(msg);
	msg_str = str;
	cout << msg;
	string logPr = "Recieved: " + msg_str + " at: " + currentDateTime();
	log << "[" << logPr.size() << "][" << logPr << "]\n";
	if (msg_str != "Symbols can be entered:") cout << endl;
	delete[] msg;
}

void Send(const SOCKET& ConnectSocket, string& msg_str, ofstream& log) { //TODO
	//while(msg.size() < 256)
	int MSGsize = msg_str.size();
	send(ConnectSocket, (char*)&MSGsize, sizeof(int), NULL);
	//cout << "sended size" << endl;
	send(ConnectSocket, msg_str.c_str(), MSGsize, NULL);
	//cout << "sended msg " << msg_str << endl;
	string logPr = "Sent: " + msg_str + " at: " + currentDateTime();
	log << "[" << logPr.size() << "][" << logPr << "]\n";
}


//Функція очистки та закриття програми
void CleanUpAndClose(SOCKET s) {
	closesocket(s);
	WSACleanup();
	system("pause");
	exit(0);
}

int main(int argc, char* argv[]) {

	//WSAStartup - функція для завантаження біблеотки
	WSAData WSA_Socket; //вказівник на стурктуру з деякою інформацією
	WORD DLLVersion = MAKEWORD(2, 2); // Робиму змінну в якій за допомогую функції буде запис числа версії бібліотеки в 16-ковій системі
	if (WSAStartup(DLLVersion, &WSA_Socket) != 0) { // а що в WSA_Socket?
		cout << "Error at WSAStartup()" << endl;
		system("pause");
		return 0;
	}
	else {
		cout << "No Error at WSAStartup()" << endl;
	}

	//Дані з якими треба буде зв'язати майбутній сокет
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1050); //1025 + 25(Варіант)

	//Під'єднуємося до сервера
	SOCKET ConnectSocket = socket(PF_INET, SOCK_STREAM, NULL);
	if (connect(ConnectSocket, (SOCKADDR*)&addr, *(&sizeofaddr)) == SOCKET_ERROR) {
		cout << "Failed to connect" << endl;
		CleanUpAndClose(ConnectSocket);
		system("pause");
	}
	cout << "Connected to server." << endl;

	//Надсилаємо повідомлення
	ofstream log("ClientLog.txt");
	if (log.is_open() == true) {
		cout << "Log is open!" << endl;
	}
	else {
		cout << "Log is not open!" << endl;
	}

	string msg;
	Recv(ConnectSocket, msg, log);
	while (1) {
		getline(cin, msg, '\n');
		if (msg == "Who") {
			Send(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
		}
		else if (msg == "Send") {
			Send(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
			Recv(ConnectSocket, msg, log);
			int n = stoi(msg);
			char c;
			for (int i = 0; i < n; i++) {
				cin >> c;
				msg = c;
				Send(ConnectSocket, msg, log);
			}
			cout << "All symbols are entered. Here is the result: ";
			break;
		}
		else if (msg == "Break") {
			Send(ConnectSocket, msg, log);
			break;
		}
		else {
			cout << "WRONG COMMAND" << endl;
		}
	}
	Recv(ConnectSocket, msg, log);
	log.close();
	if (log.is_open() != true) {
		cout << "Log is closed!" << endl;
	}
	else {
		cout << "Log is not closed!" << endl;
	}
	system("pause");
	return 0;
}*/