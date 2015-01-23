# 自动产生依赖 生成需要的文件
# 
#目标
TARGET=main
#所在头文件的目录
INC=-I.
#源文件
VPATH=.
LIBS=
CFLAGS=
C=gcc

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

#编译成目标文件
$(TARGET) : $(OBJS)
	@echo "srcs:" $(SRCS)
	@echo "objs:" $(OBJS)
	@echo "complie to objs"
	$(CC) $(OBJS) $(CFLAGS) $(LIBS) $(INC) -o $(TARGET)

#sinclude $(SRCS:.c=.d)

#%.d : %.c
	#$(CC) -MM $(CFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@&nbsp;rm -f $@.$$$$

clean:
	rm -f $(OBJS) $(TARGET) 
