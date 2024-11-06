#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define DEFAULT_PORT "1278"

using namespace cv;

class Server {
public:
	Server() {
		buf = new uchar[1920 * 1080 * 4];
		WSAData data;
		int iResult = WSAStartup(MAKEWORD(2, 2), &data);
		if (iResult != 0) {
			fprintf(stderr, "Couldn`t initialize WinSockets. Error code: %d\n", WSAGetLastError());
			exit(1);
		}

		struct addrinfo* result = nullptr, * ptr = nullptr, hints;
		ZeroMemory(&hints, sizeof(hints));

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("GetAddrInfo() failed with code: %d\n", iResult);
			WSACleanup();
			exit(1);
		}

		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (ListenSocket == INVALID_SOCKET) {
			iResult = WSAGetLastError();
			printf("socket() failed with code: %d\n", iResult);
			freeaddrinfo(result);
			WSACleanup();
			exit(2);
		}
		int enable = 1;
		setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) & enable, 1);
		iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			iResult = WSAGetLastError();
			printf("bind() failed with code: %d\n", iResult);
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			exit(3);
		}
		freeaddrinfo(result);
		iResult = listen(ListenSocket, SOMAXCONN);
		if (ListenSocket == SOCKET_ERROR) {
			iResult = WSAGetLastError();
			printf("listen() failed with code: %d\n", iResult);
			closesocket(ListenSocket);
			WSACleanup();
			exit(4);
		}
		ClientSocket = accept(ListenSocket, nullptr, nullptr);
		if (ClientSocket == INVALID_SOCKET) {
			iResult = WSAGetLastError();
			printf("accept() failed with code: %d\n", iResult);
			closesocket(ListenSocket);
			WSACleanup();
			exit(5);
		}
		printf("Accepted\n");
	}
	~Server() {
		closesocket(ClientSocket);
		closesocket(ListenSocket);
		WSACleanup();
	}

	void ReceiveImage() {
		int len = recv(ClientSocket, (char *)buf, 1920 * 1080 * 4, 0);
		img = std::vector<uchar>(buf, buf + len);
	}

	const std::vector<uchar>& GetImage()
	{
		return img;
	}

private:	
	uchar* buf;
	std::vector<uchar> img;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
};

int main(int argc, char** argv)
{
	Server sv;

	sv.ReceiveImage();
	const std::vector<uchar>& buf = sv.GetImage();
	Mat frame = imdecode(buf, IMWRITE_JPEG_QUALITY);
	int codec = VideoWriter::fourcc('D', 'I', 'V', 'X');
	VideoWriter writer("output.avi", codec, 12.0f, frame.size(), frame.type() == CV_8UC3);

	while (true)
	{
		std::vector<int> params(2);
		params[0] = IMWRITE_JPEG_QUALITY;
		params[1] = 95;

		sv.ReceiveImage();
		const std::vector<uchar>& buf = sv.GetImage();

		frame = imdecode(buf, IMWRITE_JPEG_QUALITY);

		imshow("Server", frame);
		writer.write(frame);

		switch (waitKey(25)) {
			case 'q':
				return 0;
			case 's':
				imwrite("example.jpeg", frame);
				break;
		}
	}
	
	return 0;
}
