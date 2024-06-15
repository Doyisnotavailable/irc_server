NAME = irc
SRC_DIR = ./src
SRC = 
OBJ_DIR = ./obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
$(shell mkdir -p $(OBJ_DIR))
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)


