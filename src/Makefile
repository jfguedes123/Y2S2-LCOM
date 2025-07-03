# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}/devices/
.PATH: ${.CURDIR}/devices/keyboard
.PATH: ${.CURDIR}/devices/mouse
.PATH: ${.CURDIR}/devices/keyboard
.PATH: ${.CURDIR}/devices/mouse
.PATH: ${.CURDIR}/devices/timer
.PATH: ${.CURDIR}/devices/video
.PATH: ${.CURDIR}/devices/kbc
.PATH: ${.CURDIR}/utils
.PATH: ${.CURDIR}/xpm_files
.PATH: ${.CURDIR}/game
# source code files to be compiled
SRCS = main.c KBC.c video.c utils.c keyboard.c mouse.c timer.c play.c rules.c state.c menu.c digits.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf -lxpm -llm

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>