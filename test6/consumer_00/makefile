#export BIN_PATH=${CUR_PATH}/build/bin
export LIB_PATH=../


TARGET := ${$(pwd)##*/}    #找到最后一个/并去掉左边

OBJDIR := ./
SRCDIR := ./


BIN_PATH := ../



CC := gcc


CFLAGS  := -I ../
LDFLAGS := -lsem_shm_api

FILTERSOURCES :=

#这里递归遍历5级子目录
DIRS := $(shell find $(SRCDIR) -maxdepth 1 -type d)
#查找src_dir下面包含子目录的所有c文件
SOURCES   = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
#             加前缀                 将.c替换为.o      去掉目录  过滤FILTERSOURCES
OBJECTS   = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(notdir $(filter-out $(FILTERSOURCES),$(SOURCES)))))

# 表示对应的.d 
DEPENDS := $(OBJECTS:.o=.d)


#创建obj路径
#$(shell if [ ! -d obj ];then mkdir obj; fi)



all: ${BIN_PATH}/${TARGET} 
#检查make参数
ifneq ($(MAKECMDGOALS),clean)
-include ${DEPENDS}
endif


${BIN_PATH}/${TARGET}: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} ${LDFLAGS}

${OBJDIR}/%.o:%.c
	${CC} -o $@ -c $< ${CFLAGS}

${OBJDIR}/%.d:%.c
	rm -f $@
	${CC} -MM $< > $@.temp ${CFLAGS}
	sed 's,\($*\)\.o[:]*,${OBJDIR}/\1.o $@:,g' < $@.temp >$@
	rm -rf $@.temp


.PHONY : clean
clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d ${BIN_PATH}/${TARGET}


