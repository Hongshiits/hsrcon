#include<iostream>
#include<WinSock2.h>
#include<thread>
#include<string>

#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

//#define _HEAD_

using namespace std;

int32_t id_gol=0;
SOCKET soc_gol;

void init_soc(const char* ip_in,int port_in) {
	WORD w_req = MAKEWORD(2, 2);
	WSADATA wsadata;

	WSAStartup(w_req, &wsadata);
	soc_gol = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip_in);
	server_addr.sin_port = htons(port_in);


	connect(soc_gol, (SOCKADDR*)&server_addr, sizeof(SOCKADDR));
}

string rcon_recv(SOCKET soc,int32_t &back_id) {
	int32_t pack_size = 0;
	char* temp_size=new char[4];
	//system("pause");
	recv(soc, temp_size, 4, 0);
	memcpy(&pack_size, temp_size, 4);
	//cout << (int32_t)pack_size << endl;

	char* buf_rec = new char[pack_size];
	if (recv(soc, buf_rec, pack_size, 0) < 0) {
		return NULL;
	}
	//back_id = *(buf_rec + 4);
	//printf("in_func %d \n", (int)*(buf_rec));
	int32_t temp_in=0;
	memcpy(&temp_in, buf_rec, 4);
	back_id = temp_in;
	return string(buf_rec + 12);
}


void rcon_recv_loop(SOCKET soc) {
	while (1) {
		try {
			int32_t temp;
			cout<<rcon_recv(soc,temp)<<endl;
		}
		catch (exception e) {
			return;
		}
	}
}

int rcon_send(SOCKET soc,int32_t type_in,string str_in) {
	int32_t pack_id = (++id_gol), pack_type = type_in, pack_size;
	char* pack_body;
	char pack_end = 0;

	string send_str(str_in);
	//pack_body = new char[send_str.size()];
	pack_body = (char*)send_str.c_str();
	pack_size = send_str.size() +8;

	char* send_buf = new char[pack_size + 6];
	memset(send_buf, 0, pack_size + 6);
	*send_buf = pack_size+2;
	*(send_buf + 4) = pack_id;
	*(send_buf + 8) = pack_type;
	//*(send_buf + 12) = *pack_body;
	strcpy(send_buf + 12, pack_body);
	*(send_buf + 4 + pack_size) = (char)0;

	//cout << send_str.size() <<endl<<send_str<<endl<<pack_body<<endl <<pack_size<<endl<<pack_id<<endl<<pack_type<<endl<< send_buf << endl;
	//system("pause");

	return send(soc, send_buf, pack_size + 6, 0);
	
}

int rcon_auth(const char* passwd) {
	rcon_send(soc_gol, 3, passwd);
	//system("pause");

	int32_t temp = -1;
	rcon_recv(soc_gol, temp);
	//cout << id_gol << endl << temp<<endl;
	if (temp != id_gol) {
		cout << "auth fail" << endl;
		return 0;
	}
	return 1;
}

void close_soc() {
	closesocket(soc_gol);
	WSACleanup();
	system("pause");
}

#ifndef _HEAD_

int main() {
	
	init_soc("127.0.0.1", 25575);
	//cout << "finish init" << endl;

	//recv auto message
	
	//cout << rcon_recv(soc, temp) << endl;
	
	//auth
	string passwd;
	cout << "enter passwd:";
	cin >> passwd;
	if (rcon_auth(passwd.c_str()) == 0) {
		system("pause");
		return 0;
	}
	

	thread recv_th(rcon_recv_loop, soc_gol);
	while (1) {
		Sleep(100);
		string input_send;
		cout << "@";
		cin >> input_send;
		if (rcon_send(soc_gol, 2, input_send) < 0) {
			cout << "send fail" << endl;
			system("pause");
			return 0;
		}

	}

	
	
	close_soc();

	
	return 0;
}
#endif // !_HEAD_