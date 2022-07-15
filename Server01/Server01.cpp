#include <stdio.h>
#include<iostream>
#include "winsock2.h"
#include <string>
#include<ctime>
#include <cstdlib>
#include<algorithm>
#include <fstream>
#include <cctype> 
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
	recv(Connection, msg, msg_size, NULL);//Приймаємо повідомлення
	string str(msg);
	msg1 = str;

	string logPr = "Recieved: " + msg1 + " at: " + currentDateTime();// записуємо в журнал подій
	log << "[" << logPr.size() << "]: [" << logPr << "]\n";

	delete[] msg;
}


int main() {

	ifstream file1("1S.txt");
	ifstream file2("2S.txt");

	WSADATA wsaDATA;
	WORD DLLVersion = MAKEWORD(2, 2);
	if (WSAStartup(DLLVersion, &wsaDATA) != 0) {
		cout << "Error at WSAStartup()" << endl;
		system("pause");
		return 0;
	}

	SOCKADDR_IN service;
	int sizeofservice = sizeof(service);
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(1044);		//1025+19

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (ListenSocket == INVALID_SOCKET) {
		cout<<"Error at socket()" <<endl;
		WSACleanup();
	}
	else {
		cout << "No Error at socket()" << endl;
	}

	if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		cout << "bind() failed" << endl;
		closesocket(ListenSocket);
	}
	else {
		cout << "No Error at bind()" << endl;
	}

	if (listen(ListenSocket, 1) == SOCKET_ERROR) {
		cout << "Error listening on socket" << endl;
		WSACleanup();
	}
	else {
		cout << "No Error at listening()" << endl;
	}

	//Підключаємо клієнта до сервера
	SOCKET AcceptSocket;
	cout <<"Waiting for client to connect..."<< endl;
	AcceptSocket = SOCKET_ERROR;
	while (AcceptSocket == SOCKET_ERROR) {
		AcceptSocket = accept(ListenSocket,(SOCKADDR*)&service, &sizeofservice);
	}

	if (AcceptSocket == 0) {
		cout << "Error accept()" << endl;
	}
	else {
		cout << "Client connected" << endl;
		ListenSocket = AcceptSocket;
	}

	//Починаємо роботу із клієнтом
	ofstream log("ServerLog.txt");//you should create "ServerLog.txt"
	if (log.is_open() == true) {
		cout << "Log is open!" << endl;
	}
	else {
		cout << "Log is not open!" << endl;
	}

	string msg = "Enter command:\nWho/FileName.txt/Break\n"; 
	Send(ListenSocket, msg, log);
	while (1) {
		Recv(ListenSocket, msg, log);
		if (msg == "Who") {
			msg = "K-24 Nikitina Sophie";
			Send(ListenSocket, msg, log);
		}
		else if (msg == "Break") {
			break;
		}
		else {
			transform(msg.begin(), msg.end(), msg.begin(),[](unsigned char c) { return tolower(c); });
			string n = "1S.txt", m="2S.txt";
			string msgforcomp = convertFiletoString(n);
			transform(n.begin(), n.end(), n.begin(), [](unsigned char c) { return tolower(c); });
			string msgforcomp2 = convertFiletoString(m);
			transform(m.begin(), m.end(), m.begin(), [](unsigned char c) { return tolower(c); });

			if (msg == msgforcomp) {
				msg = "You message is quaziidentifical with \"1S.txt\"";
				Send(ListenSocket, msg, log);
			}else if (msg == msgforcomp2) {
				msg = "You message is quaziidentifical with \"2S.txt\"";
				Send(ListenSocket, msg, log);
			}
			else {
				msg = "You message isn't quaziidentifical";
				Send(ListenSocket, msg, log);
			}
		}
	}

	log.close();
	if (log.is_open() != true) {
		cout << "Log is closed!" << endl;
	}
	else {
		cout << "Log is not closed!" << endl;
	}

	closesocket(AcceptSocket);
	WSACleanup();
	system("pause");
	return 0;
}

