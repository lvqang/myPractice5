gcc pc.c sem_shm.c -o pc -I sem_shm.h
cd test6
gcc ../sem_shm.c consumer_0x.c -o consumer_0x -I ../sem_shm.h

TARGET    := pc
LDTAEGET  := libsem_shm_api.so
OBJDIR := ./obj
SRCDIR := ./
#
BIN_PATH := .
LIB_PATH := .
#
#
CC := gcc
#
#
CFLAGS  := -I ./
LDFLAGS := -lsem_shm_api
#
FILTERSOURCES :=
#
#这里递归遍历5级子目录
DIRS := $(shell find $(SRCDIR) -maxdepth 1 -type d)
#查找src_dir下面包含子目录的所有c文件
SOURCES   = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
#             加前缀                 将.c替换为.o      去掉目录  过滤FILTERSOURCES
OBJECTS   = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(notdir $(filter-out $(FILTERSOURCES),$(SOURCES)))))
#
# 表示对应的.d 
#DEPENDS := $(OBJECTS:.o=.d)
#
#
#创建obj路径
#$(shell if [ ! -d obj ];then mkdir obj; fi)
#
#
all: ${BIN_PATH}/${TARGET} build
${BIN_PATH}/${TARGET}:${BIN_PATH}/pc.c ${BIN_PATH}/sem_shm.c
	${CC} $^ -o $@
#${BIN_PATH}/%o:%c
#	${CC} -o $@ -c $^
build:
	$(shell if [ ! -d consumer_00 ];then cd consumer_00; make all; fi)
	$(shell if [ ! -d consumer_01 ];then cd consumer_01; make all; fi)
	$(shell if [ ! -d consumer_02 ];then cd consumer_02; make all; fi)
	$(shell if [ ! -d consumer_03 ];then cd consumer_03; make all; fi)
	$(shell if [ ! -d consumer_04 ];then cd consumer_04; make all; fi)
	$(shell if [ ! -d consumer_05 ];then cd consumer_05; make all; fi)
#	
#
.PHONY : clean
clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d ${BIN_PATH}/${TARGET} ${LIB_PATH}/${LDTAEGET}
	$(shell if [ ! -d consumer_00 ];then cd consumer_00; make clean; fi)
	$(shell if [ ! -d consumer_01 ];then cd consumer_01; make clean; fi)
	$(shell if [ ! -d consumer_02 ];then cd consumer_02; make clean; fi)
	$(shell if [ ! -d consumer_03 ];then cd consumer_03; make clean; fi)
	$(shell if [ ! -d consumer_04 ];then cd consumer_04; make clean; fi)
	$(shell if [ ! -d consumer_05 ];then cd consumer_05; make clean; fi)
