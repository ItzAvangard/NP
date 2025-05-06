all: udp_chat_server udp_chat_client tcp_file_sender tcp_file_receiver

udp_chat_server: udp_chat_server.cpp
	g++ udp_chat_server.cpp -o udp_chat_server

udp_chat_client: udp_chat_client.cpp
	g++ udp_chat_client.cpp -o udp_chat_client -pthread

tcp_file_sender: tcp_file_sender.cpp
	g++ tcp_file_sender.cpp -o tcp_file_sender

tcp_file_receiver: tcp_file_receiver.cpp
	g++ tcp_file_receiver.cpp -o tcp_file_receiver