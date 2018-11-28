// klient.cpp: definiuje punkt wejœcia dla aplikacji konsolowej.
//

#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning (disable:4996)
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <ctime>
#include <map>
#include <vector>

using namespace std;

SOCKET Connection;

class packet
{
public:
	map<string, string> buffer;

	packet() {}
	packet(string OperaC, string StatuS, string IdentY, string Argum1, string Argum2, string Argum3)
	{
		buffer["OperaC"] = OperaC;
		buffer["StatuS"] = StatuS;
		buffer["IdentY"] = IdentY;
		buffer["Argum1"] = Argum1;
		buffer["Argum2"] = Argum2;
		buffer["Argum3"] = Argum3;
	}

	packet(string buffer)
	{
		std::string element = "";
		int j = 0;
		for (int i = 0; i < 6; i++)
		{
			element = "";

			if (i == 0)
				j += 8;
			else
				j += 9;

			while (buffer[j] != '^')
			{
				element += buffer[j];
				j++;
			}

			switch (i)
			{
			case 0:
				this->buffer["OperaC"] = element;
				break;
			case 1:
				this->buffer["StatuS"] = element;
				break;
			case 2:
				this->buffer["IdentY"] = element;
				break;
			case 3:
				this->buffer["Argum1"] = element;
				break;
			case 4:
				this->buffer["Argum2"] = element;
				break;
			case 5:
				this->buffer["Argum3"] = element;
				break;
			}
		}
	}

	void display()
	{
		cout << "OperaC>>" << buffer["OperaC"] << " StatuS>>" << buffer["StatuS"] << " IdentY>>" << buffer["IdentY"]
			<< " Argum1>>" << buffer["Argum1"] << " Argum2>>" << buffer["Argum2"] << " Argum3>>" << buffer["Argum3"];
		cout << endl;
	}

	string toString()
	{
		string converted_buffer;
		string OperaC = "OperaC>>" + buffer["OperaC"] + "^";
		string StatuS = "StatuS>>" + buffer["StatuS"] + "^";
		string IdentY = "IdentY>>" + buffer["IdentY"] + "^";
		string Argum1 = "Argum1>>" + buffer["Argum1"] + "^";
		string Argum2 = "Argum2>>" + buffer["Argum2"] + "^";
		string Argum3 = "Argum3>>" + buffer["Argum3"] + "^";
		converted_buffer = OperaC + StatuS + IdentY + Argum1 + Argum2 + Argum3;

		return converted_buffer;
	}

	string getID()
	{
		return buffer["IdentY"];
	}

	string getArg1()
	{
		return buffer["Argum1"];
	}


};


int main()
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) { //Jezeli WSAStartup zwroci cokolwiek innego od 0 to wystapil blad w startupie
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Adres ktory zwiazemy z socketem polaczenia
	int addrlen = sizeof(addr); //Potrzeba sizeof do funkcji connect
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Adres = localhost
	addr.sin_port = htons(1111); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket

	Connection = socket(AF_INET, SOCK_STREAM, NULL); //ustawianie socketu
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{ //Jezeli nie mozemy sie polaczyc
		time_t t = time(0);
		struct tm* now = localtime(&t);
		cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Nie udalo sie polaczyc z serwerem, zrestartuj aplikacje.\n";
		system("PAUSE");
		return 0; //Nie udalo sie polaczyc
	}
	time_t t = time(0);
	struct tm* now = localtime(&t);
	cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Polaczono z serwerem" << endl;

	//OTRZYMYWANIE PAKIETU Z ID
	packet received;
	char buffer_in[256];
	recv(Connection, buffer_in, 256, NULL);
	received = packet(string(buffer_in));
	string id = received.getID();
	cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Otrzymano pakiet: ";
	received.display();
	cout << "\nOtrzymane ID: " << id << endl;;

	string input, op;
	int arg1, arg2, arg3;
	while (true)
	{
		cout << "Podaj jaka operacje chcesz wykonac:\nop1 - odejmowanie\nop2 - dzielenie\nop3 - dodawanie\nop4 - mnozenie\n";
		while (cin >> input)
		{
			if (input == "op1") { op = "odejmowanie"; break; }
			else if (input == "op2") { op = "dzielenie"; break; }
			else if (input == "op3") { op = "dodawanie"; break; }
			else if (input == "op4") { op = "mnozenie"; break; }
			else { cout << "Niepoprawnie wpisano typ operacji. Sprobuj ponownie: "; continue; }
		}
		cout << "Argument nr 1: ";
		if (input == "op2")
		{
			while (cin >> arg1)
			{
				if (arg1 == 0) { cout << "Nie mozna dzielic przez 0. Podaj poprawna wartosc: "; continue; }
				else if (arg1 != 0) { break; }
			}
		}
		else { cin >> arg1; }
		cout << "Argument nr 2: ";
		if (input == "op2")
		{
			while (cin >> arg2)
			{
				if (arg2 == 0) { cout << "Nie mozna dzielic przez 0. Podaj poprawna wartosc: "; continue; }
				else if (arg2 != 0) { break; }
			}
		}
		else { cin >> arg2; }
		cout << "Argument nr 3: ";
		if (input == "op2")
		{
			while (cin >> arg3)
			{
				if (arg3 == 0) { cout << "Nie mozna dzielic przez 0. Podaj poprawna wartosc: "; continue; }
				else if (arg3 != 0) { break; }
			}
		}
		else { cin >> arg3; }
		string arg1_str = to_string(arg1);
		string arg2_str = to_string(arg2);
		string arg3_str = to_string(arg3);
		time_t t = time(0);
		struct tm* now = localtime(&t);
		cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Wysylam pakiet: ";
		packet buffer_out(op, "zapytanie", id, arg1_str, arg2_str, arg3_str);
		buffer_out.display();
		send(Connection, buffer_out.toString().c_str(), buffer_out.toString().size(), NULL);

		packet received;
		char buffer_in[256];
		recv(Connection, buffer_in, 256, NULL);
		received = packet(string(buffer_in));
		t = time(0);
		now = localtime(&t);
		cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Otrzymano pakiet: ";
		received.display();
		double temp = atof(received.getArg1().c_str());
		cout << "Wynik: " << temp << endl;

		shutdown(Connection, 2); //0 - odbior, 1 - zapis, 2 - odbior i zapis
		if (shutdown(Connection, 2) == 0)
		{ //shutdown zwraca 0 jezeli uda sie rozlaczyc
			cout << "Rozlaczono z serwerem.\n";
			break;
		}
		Sleep(10);
	}

	system("PAUSE");
	return 0;
}




//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);
/*
shutdown(Connection, 2); //0 - odbior, 1 - zapis, 2 - odbior i zapis
if (shutdown(Connection, 2) == 0) { //shutdown zwraca 0 jezeli uda sie rozlaczyc
std::cout << "Rozlaczono z serwerem\n";
}
WSACleanup();
*/

//std::cin.getline(buffer, sizeof(buffer));//lapie to co wpisze klient
//send(Connection, buffer, sizeof(buffer), NULL);


//Zamykanie polaczenia
//1 sposob
//closesocket(Connection);
//WSACleanup();
//2 sposob
//shutdown(Connection, 2); //0 - odbior, 1 - zapis, 2 - odbior i zapis
/*if (shutdown(Connection, 2) == 0) { //shutdown zwraca 0 jezeli uda sie rozlaczyc
std::cout << "Rozlaczono z serwerem\n";
}
WSACleanup();*/

/*
void ClientThread()
{
char buffer[256];
while (true)
{
//recv(Connection, buffer, sizeof(buffer), NULL);
//std::cout << buffer << std::endl;

}
}
*/