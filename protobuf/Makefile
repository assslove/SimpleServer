all : test_proto
test_proto: test_proto.cpp ./proto/lib/simple.pb.cc
	g++ test_proto.cpp ./proto/lib/simple.pb.cc -o test_proto `pkg-config --cflags --libs protobuf` -I/proto/lib 
clean:
	rm -rf test_proto
