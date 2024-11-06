#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define DEFAULT_PORT "1278"

using namespace cv;

class Client
{
public:
	Client()
	{
		WSAData data;
		int res = WSAStartup(MAKEWORD(2, 2), &data);
		if (res != 0) {
			printf("WSAStartup() failed\n");
			exit(1);
		}

		struct addrinfo* result = nullptr,
			* ptr = nullptr,
			hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		
		res = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
		if (res != 0) {
			int code = WSAGetLastError();
			printf("GetAddrInfo() failed with code %d\n", code);
			freeaddrinfo(result);
			WSACleanup();
			exit(2);
		}
		ptr = result;
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		res = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (res == SOCKET_ERROR) {
			int code = WSAGetLastError();
			printf("Connect() failed with code %d\n", code);
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
		freeaddrinfo(result);
		if (ConnectSocket == INVALID_SOCKET) {
			WSACleanup();
			exit(3);
		}
		printf("Connected\n");
	}

	~Client() {
		if (ConnectSocket != INVALID_SOCKET) {
			closesocket(ConnectSocket);
		}
		WSACleanup(); 
	}

	void SendImage(const std::vector<uchar>& buf)
	{
		int res = send(ConnectSocket, (const char*)buf.data(), buf.size(), 0);
		if (res == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			WSACleanup();
		}
	
	}
	
private:
	SOCKET ConnectSocket = INVALID_SOCKET;
};

int main(int argc, char** argv)
{
	Client me;
	
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		fprintf(stderr, "Capture isn`t opened\n");
		return 1;
	}
	
	Mat frame;
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	cap.read(frame);
	
	while (true) {
		bool ret = cap.read(frame);
		if (!ret) {
			fprintf(stderr, "Can`t receive frame(stream end?). Exiting...\n");
			break;
		}
	
		std::vector<int> params(2);
		params[0] = IMWRITE_JPEG_QUALITY;
		params[1] = 95;

		std::vector<uchar> buf;
		imencode(".jpeg", frame, buf, params);
		me.SendImage(buf);

		imshow("Client", frame);

		switch (waitKey(25)) {
			case 'q':
				return 0;
			case 's':
				imwrite("example.jpeg", frame);
				break;
		}
	}
	cap.release();

	return 0;
}
