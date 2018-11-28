// serwer.cpp: definiuje punkt wejœcia dla aplikacji konsolowej.
//

#include "pch.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning (disable:4996)
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <map>
#include <string>

using namespace std;

//now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " <<

SOCKET Connections[100]; //mozna przyjac max 100 polaczen
int ConnectionCounter = 0;//licznik polaczenia, inkrementowany z kazdym nowym polaczeniem
int id = 1;

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

	string getOP()
	{
		return buffer["OperaC"];
	}

	string getArg1()
	{
		return buffer["Argum1"];
	}

	string getArg2()
	{
		return buffer["Argum2"];
	}

	string getArg3()
	{
		return buffer["Argum3"];
	}
};

void ClientHandlerThread(int index)
{
	packet buffer_out;
	packet received;
	char buffer_in[256];

	//WYSYLANIE ID
	time_t t = time(0);
	struct tm* now = localtime(&t);
	string str_id = to_string(id);
	cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Wysylam pakiet: ";
	buffer_out = { "nadawanie_id","wysylanie_id", str_id, "0","0","0" };
	buffer_out.display();
	send(Connections[index], buffer_out.toString().c_str(), buffer_out.toString().size(), NULL);
	id++;


	double a, b, c;
	double wynik;
	while (true)
	{
		time_t t = time(0);
		struct tm* now = localtime(&t);
		recv(Connections[index], buffer_in, 256, NULL);
		received = packet(string(buffer_in));
		cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Otrzymano pakiet (ID: " << received.getID() << "): ";
		received.display();

		a = stoi(received.getArg1());
		b = stoi(received.getArg2());
		c = stoi(received.getArg3());

		if (received.getOP() == "odejmowanie") { wynik = (a - b - c); }
		else if (received.getOP() == "dzielenie") { wynik = ((a / b) / c); }
		else if (received.getOP() == "dodawanie") { wynik = (a + b + c); }
		else if (received.getOP() == "mnozenie") { wynik = (a * b * c); }
		t = time(0);
		now = localtime(&t);
		cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Wysylam pakiet: ";
		buffer_out = { "wynik","odpowiedz", received.getID(), to_string(wynik),"0","0" };
		buffer_out.display();
		send(Connections[index], buffer_out.toString().c_str(), buffer_out.toString().size(), NULL);

		if (Connections[index] != 0)
		{
			t = time(0);
			now = localtime(&t);
			cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Klient (ID: " << received.getID() << ") rozlaczyl sie.\n";
			shutdown(Connections[index], 2); //0 - odbior, 1 - zapis, 2 - odbior i zapis
			if (shutdown(Connections[index], 2) == 0)
			{ //0= udalo sie rozlaczyc
				t = time(0);
				now = localtime(&t);
				cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " " << "Zamknieto polaczenie z klientem (ID: " << received.getID() << ").";
				break;
			}
		}
	}
}

int main()
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{ //Jezeli WSAStartup zwroci cokolwiek innego od 0 to wystapil blad w startupie
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Adres z ktorym zwiazemy nasluchujacy socket
	int addrlen = sizeof(addr); //dlugosc adresu (wymagana do accept call)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast lokalny
	addr.sin_port = htons(1111); //Port na ktorym serwer nasluchuje
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Tworzenie socketa do nasluchiwania nowego polacenia
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); //Zwiaz adres z socketem
	listen(sListen, SOMAXCONN); //Zmienia stan socketu sListen na nasluchiwanie nadchodzacego polaczenia

	SOCKET newConnection; //Socket do trzymania polaczenia klienta
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Zaakceptuj nowe polaczenie
		if (newConnection == 0)
		{ //Jezeli akceptowanie polaczenia klienta nie powiodlo sie
			time_t t = time(0);
			struct tm* now = localtime(&t);
			cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << "Nie udalo sie akceptowac polaczenia klienta." << std::endl;
		}
		else
		{ //Jezeli polaczenie klienta zostalo poprawnie zaakceptowane
			time_t t = time(0);
			struct tm* now = localtime(&t);
			cout << now->tm_hour << ':' << now->tm_min << ':' << now->tm_sec << " Polaczono z klientem. ID klienta: " << id << std::endl;
			Connections[i] = newConnection;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL);//Tworzy watek zajmujacy sie obecnym klientem. (i) to index w tablicy polaczen dla tego watku.
			ConnectionCounter++;
		}
	}

	system("PAUSE");
	return 0;
}






/*

WSACleanup();
}
*/