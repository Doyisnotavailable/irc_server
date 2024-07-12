NAME                    = ircserv
BONUS_NAME              = BOT

RM                      = rm -rf
CXX                     = c++
CXXFLAGS                = -Wall -Wextra -Werror -std=c++98 -g3

OBJDIR                  = obj
SRCS                    = src/main.cpp src/Server.cpp src/Errormsg.cpp src/Client.cpp src/Channel.cpp src/Util.cpp
BONUS_SRCS              = src/Server.cpp src/Errormsg.cpp src/Client.cpp src/Channel.cpp src/Util.cpp bonus/main.cpp bonus/BOT.cpp

OBJ_FILES               = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
BONUS_OBJ_FILES         = $(addprefix $(OBJDIR)/, $(BONUS_SRCS:.cpp=.o))

$(OBJDIR)/%.o:  %.cpp
						mkdir -p $(dir $@)
						$(CXX) $(CXXFLAGS) -c $< -o $@

all:                    $(NAME)

bonus:                  $(BONUS_NAME)

$(NAME):                $(OBJ_FILES)
						$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(NAME)

$(BONUS_NAME):          $(BONUS_OBJ_FILES)
						$(CXX) $(CXXFLAGS) $(BONUS_OBJ_FILES) -o $(BONUS_NAME)

clean:
						$(RM) $(OBJ_FILES)
						$(RM) $(OBJDIR)
						$(RM) $(BONUS_OBJ_FILES)

fclean:                 clean
						$(RM) $(NAME)
						$(RM) $(BONUS_NAME)

re:                     fclean $(NAME)

push:                   fclean
						git add .
						git commit -m "Updated on $(shell date +'%Y-%m-%d %H:%M:%S') by $(USER)"
						git push

.PHONY:                 all clean fclean re push

