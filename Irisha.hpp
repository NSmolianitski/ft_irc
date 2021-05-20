//
// Created by Parfait Kentaurus on 5/6/21.
//

#ifndef FT_IRC_IRISHA_HPP
#define FT_IRC_IRISHA_HPP

#include "AConnection.hpp"
#include "User.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <list>

#include <unistd.h>
#include <netinet/in.h>

#define CONFIG_PATH "irisha.conf"
#define NO_PREFIX	""

struct Command
{
	std::string					prefix_;
	std::string					command_;
	std::vector<std::string>	arguments_;
};

enum eResult
{
	R_SUCCESS,
	R_FAILURE
};

class Irisha
{
private:
	struct RegForm
	{
		int		socket_;
		bool	pass_received_;

		explicit RegForm(int sock)
		{
			socket_ = sock;
			pass_received_ = false;
		}
	};

	typedef eResult (Irisha::*func)(const int sock);

	int			listener_;
	sockaddr_in	address_;
	char		buff_[512];
	fd_set		all_fds_;
	fd_set		read_fds_;
	fd_set		serv_fds_;
	int			max_fd_;
    Command		cmd_;			// Struct for parsed command
	std::string	host_name_;		// Host server. Need when this server connected to other.
	std::string	password_;		// Password for clients and servers connection to connect this server
	time_t		launch_time_;	// Server launch time
	int 		parent_fd_;

	std::map<std::string, AConnection*>		connections_;	// Server and client connections
	std::map<std::string, func>				commands_;		// IRC commands

	/// Configuration members
	std::string	domain_;        // Server name
	std::string	welcome_;       // Welcome message
	int			ping_timeout_;  // How often server sends PING command
	int			conn_timeout_;	// Seconds without respond until disconnection

	std::list<Irisha::RegForm*>::iterator	expecting_registration(int i, std::list<RegForm*>& reg_expect);
	int										register_connection	(std::list<RegForm*>::iterator rf);

	/// Useful typedefs
	typedef std::map<std::string, AConnection*>::iterator		con_it;
	typedef std::map<std::string, AConnection*>::const_iterator	con_const_it;

	/// Initialization
	void			prepare_commands	();
	void			launch				();
	void 			init				(int port);
	void			apply_config		(const std::string& path);
	void			loop				();

	/// Connections
	int				accept_connection	();
	void			handle_disconnection(const int sock);
	void			handle_command		(const int sock);
	AConnection*	find_connection		(const int sock) const;
	void			ping_connections	(time_t& last_ping);

	/// Users
	void			add_user			(const int sock, const std::string& nick);
	void			add_user			();
	void			remove_user			(const std::string& nick);
	User*			find_user			(const std::string& nick) const;
	User*			find_user			(const int sock) const;

	/// Servers
	Server*			find_server			(const std::string& name) const;
	Server*			find_server			(const int sock) const;

	/// Utils
	void			send_msg			(int sock, const std::string& prefix, const std::string& msg) const;
	void			send_rpl_msg		(int sock, eReply rpl, const std::string& msg) const;
	void			send_rpl_msg		(int sock, eError rpl, const std::string& msg) const;
	void			send_servers		(const std::string& prefix, const std::string& msg) const;
	void			send_servers		(const std::string& prefix, const std::string& msg, const int sock) const;
	void			send_everyone		(const std::string& prefix, const std::string& msg) const;
	std::string 	get_msg				(int sock);
	void			print_info			() const;
	std::string		connection_name		(const int sock) const;
	int 			next_token			();

	/// IRC commands
	eResult			NICK				(const int sock);
	eResult			USER				(const int sock);
	eResult			PASS				(const int sock);
	eResult			SERVER				(const int sock);
	eResult			PING				(const int sock);
	eResult			PONG				(const int sock);
	eResult			QUIT				(const int sock);
	eResult			TIME				(const int sock);

	/// IRC commands utils
	eResult			NICK_user			(User* const connection, const int sock, const std::string& new_nick);
	eResult			NICK_server			(const std::string& new_nick);
	std::string		createPASSmsg		(std::string password) const ;
	std::string		createSERVERmsg		(AConnection* server) const;

	/// Error replies
	void			err_nosuchserver		(const int sock, const std::string& server) const;
	void			err_nosuchnick			(const int sock, const std::string& nick) const;
	void			err_nonicknamegiven		(const int sock) const;
	void			err_nicknameinuse		(const int sock, const std::string& nick) const;
	void			err_nickcollision		(const int sock, const std::string& nick) const;
	void			err_erroneusnickname	(const int sock, const std::string& nick) const;
	void			err_nosuchchannel		(const int sock, const std::string& channel) const;
	void			err_needmoreparams		(const int sock, const std::string& command) const;
	void			err_alreadyregistered	(const int sock) const;
	void			err_noorigin			(const int sock) const;
	void			err_norecipient			(const int sock, const std::string& command) const;
	void			err_notexttosend		(const int sock) const;
	void			err_notoplevel			(const int sock, const std::string& mask) const;
	void			err_wildtoplevel		(const int sock, const std::string& mask) const;
	void			err_cannotsendtochan	(const int sock, const std::string& channel) const;
	void			err_toomanytargets		(const int sock, const std::string& target) const;
	void			err_unknowncommand		(const int sock, const std::string& command) const;
	void			err_chanoprivsneeded	(const int sock, const std::string& channel) const;
	void			err_notochannel			(const int sock, const std::string& channel) const;
	void			err_keyset				(const int sock, const std::string& channel) const;
	void			err_unknownmode			(const int sock, const std::string& mode_char) const;
	void			err_usersdontmatch		(const int sock) const;
	void			err_umodeunknownflag	(const int sock) const;
	void			err_bannedfromchan		(const int sock, const std::string& channel) const;
	void			err_initeonlychan		(const int sock, const std::string& channel) const;
	void			err_channelisfull		(const int sock, const std::string& channel) const;
	void			err_toomanychannels		(const int sock, const std::string& channel) const;
	void			err_noprivileges		(const int sock) const;
	void			err_useronchannel		(const int sock, const std::string& user, const std::string& channel) const;

	/// Common Replies
	void			rpl_welcome				(const int sock) const;
	void			rpl_youreoper			(const int sock) const;
	void			rpl_time				(const int sock, const std::string& server, const std::string& local_time) const;
	void			rpl_away				(const int sock, const std::string& nick, const std::string& away_msg) const;
	void			rpl_channelmodeis		(const int sock, const std::string& mode, const std::string& mode_params) const;
	void			rpl_banlist				(const int sock, const std::string& channel, const std::string& ban_id) const;
	void			rpl_endofbanlist		(const int sock, const std::string& channel) const;
	void			rpl_info				(const int sock, const std::string& info) const;
	void			rpl_endofinfo			(const int sock) const;
	void			rpl_motdstart			(const int sock, const std::string& server) const;
	void			rpl_motd				(const int sock, const std::string& text) const;
	void			rpl_endofmotd			(const int sock) const;
	void			rpl_umodeis				(const int sock, const std::string& mode_string) const;
	void			rpl_topic				(const int sock, const std::string& channel, const std::string& topic) const;
	void			rpl_notopic				(const int sock, const std::string& channel) const;
	void			rpl_inviting			(const int sock, const std::string& channe, const std::string& nick) const;
	void			rpl_version				(const int sock, const std::string& version, const std::string& debug_lvl
												, const std::string& server, const std::string& comments) const;

	/// Unused constructors
	Irisha				() {};
	Irisha				(const Irisha& other) {};
	Irisha& operator=	(const Irisha& other) { return *this; };

public:
	explicit Irisha	(int port);
	Irisha			(int port, const std::string& password);
	Irisha			(const std::string& host_name, int network_port, const std::string& network_password
		   						, int port, const std::string& password);
	~Irisha			();


	/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///
	enum ePrintMode
	{
		PM_LINE,
		PM_LIST,
		PM_ALL
	};

	friend void	sending_loop(const Irisha* server);
	void		print_cmd	(ePrintMode mode, const int sock) const;
	void		user_info	(const std::string& nick) const;
	void		print_user_list		() const;
	void		send_input_msg		(int sock) const;
	/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////
};

/// ‼️ ⚠️ DEVELOPMENT UTILS (REMOVE OR COMMENT WHEN PROJECT IS READY) ⚠️ ‼️ //! TODO: DEV -> REMOVE ///////
void sending_loop(const Irisha* server);
/// ‼️ ⚠️ END OF DEVELOPMENT UTILS ⚠️ ‼️ //! TODO: DEV -> REMOVE //////////////////////////////////////////

#endif //FT_IRC_IRISHA_HPP
