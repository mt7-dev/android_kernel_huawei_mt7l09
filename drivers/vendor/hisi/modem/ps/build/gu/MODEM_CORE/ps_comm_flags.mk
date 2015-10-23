#***********************************************************#
# compiler defines
#***********************************************************#
CC_USER_FLAGS :=
CC_USER_FLAGS += -c  -O2 -t7 -msoft-float -fdollars-in-identifiers \
				    -fno-builtin -fvolatile -fsigned-char -fno-feature-proxy -Wall -fno-zero-initialized-in-bss -fno-strict-aliasing

AS_USER_FLAGS  :=
AR_USER_FLAGS  +=-r -o