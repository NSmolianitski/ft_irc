
#include "Irisha.hpp"
#include "parser.hpp"

#include <cstring>
#include <sstream>

/**
 * @description	Parses command line to Command structure
 * @param		msg: message
 * @param		cmd: Command structure
 */
void 	parse_msg(const std::string& msg, Command& cmd)
{
    std::deque<std::string> array;
    std::istringstream is(msg);
    std::string s;

    cmd.prefix_.clear();
    cmd.command_.clear();
    cmd.arguments_.clear();
    while (std::getline(is, s, ' '))
        array.push_back(s);
    if (!array.empty()) {
        if (array[0][0] == ':') {
            cmd.prefix_.append(array[0].erase(0, 1));
            array.pop_front();
        }
        cmd.command_.append(array[0]);
		array.pop_front();
		while (!array.empty()) {
			if (array[0][0] == ':')
				break;
			cmd.arguments_.push_back(array[0]);
			array.pop_front();
		}
		s.clear();
		while (!array.empty())
		{
			if (!s.empty())
				s.append(" ");
			s.append(array[0]);
			array.pop_front();
		}
		if (!s.empty())
			cmd.arguments_.push_back(s);
    }
}

void parse_arr_msg(std::deque<std::string>& arr_msg, std::string& buff)
{
    std::string s;
    std::istringstream is(buff);

    while (std::getline(is, s, '\r'))
        arr_msg.push_back(s);
    if (!arr_msg.empty())
    {
        if (arr_msg.operator[](arr_msg.size() - 1) == "\n")
            arr_msg.pop_back();
        std::deque<std::string>::iterator itr = arr_msg.begin();
        while (itr != arr_msg.end())
        {
            if ((*itr).find('\n') != (*itr).npos)
                (*itr).erase((*itr).find('\n'), 1);
            itr++;
        }
    }
    buff.clear(); //! TODO: add check for command concatenation
}

void parse_argv(int argc, char *argv[], std::string& host, int& port_network, std::string& password_network, int& port, std::string& password)
{
    std::deque<std::string> array;
    std::string s;
    if (argc < 3 || argc > 4)
        throw std::runtime_error("Error size arguments");
    if (argc == 3){
        for (int i = 0; i < strlen(argv[1]); ++i) {
            if (isdigit(argv[1][i]) == 0)
                throw std::runtime_error("Error: port");
        }
        if (strlen(argv[1]) > 5 || strlen(argv[1]) < 4)
            throw std::runtime_error("Error: size port");
        port = std::atoi(argv[1]);
        password = argv[2];
    }
    else {
        std::istringstream is(argv[1]);
        while (std::getline(is, s, ':')){
            if (!s.empty())
                array.push_back(s);
        }
        if (array.size() != 3)
            throw std::runtime_error("Error size arguments network");
        host = array[0];
        array.pop_front();
        for (int i = 0; i < array[0].length(); ++i) {
            if (isdigit(array[0][i]) == 0)
                throw std::runtime_error("Error: port network");
        }
        if (array[0].length() > 5 || array[0].length() < 4)
            throw std::runtime_error("Error: size port network");
        port_network = std::stoi(array[0]);
        array.pop_front();
        password_network = array[0];
        array.pop_front();
        for (int i = 0; i < strlen(argv[2]); ++i) {
            if (isdigit(argv[2][i]) == 0)
                throw std::runtime_error("Error: port");
        }
        if (strlen(argv[2]) > 5 || strlen(argv[2]) < 4)
            throw std::runtime_error("Error: size port");
        port = std::atoi(argv[2]);
        password = argv[3];
    }
}