all:*.proto
	protoc --cpp_out=./ *.proto
	g++ -c -o user_pb.o user_info_base.pb.cc
	g++ -c -o rela_pb.o relation_info_base.pb.cc
	g++ -c -o message_pb.o message_info_base.pb.cc
	g++ -c -o photo_pb.o photo_info_base.pb.cc
	g++ -c -o cmd_def.o message_define.pb.cc
clean:
	rm *.pb.h *.pb.cc *.o
