/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abeihaqi <abeihaqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 11:43:05 by abeihaqi          #+#    #+#             */
/*   Updated: 2023/03/20 22:54:42 by abeihaqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

void		sig_ign_handler(int signum);

char	*get_env_variable(char *name)
{
	char	**env;

	env = environ;
	while (*env)
	{
		if (!ft_strncmp(name, *env, ft_strlen(name))
			&& (*env)[ft_strlen(name)] == '=')
			return (ft_strchr(*env, '=') + 1);
		env++;
	}
	return (NULL);
}

void	bsh_echo(t_cmd *cmd)
{
	int		n;
	char	*flag;

	n = 1;
	cmd->args++;
	while (*cmd->args && ft_strnstr(*cmd->args, "-", ft_strlen(*cmd->args) + 1))
	{
		flag = (*cmd->args);
		while (++flag && *flag)
		{
			if (!ft_strchr("neE", *flag))
			{
				flag = NULL;
				break ;
			}
		}
		if (!flag)
			break ;
		cmd->args++;
		n = 0;
	}
	while (*cmd->args)
	{
		ft_putstr_fd(*cmd->args, 1);
		cmd->args++;
		write(1, " ", !!(*cmd->args));
	}
	write(1, "\n", 1 * n);
}

void	bsh_chdir(t_cmd *cmd)
{
	char	*dir;
	DIR		*dirp;

	dir = *(++cmd->args);
	if (!cmd->args)
	{
		chdir(NULL);
		return ;
	}
	dirp = opendir(*cmd->args);
	if (!dirp)
		perror("bash: cd");
	else
		closedir(dirp);
	chdir(dir);
}

void	bsh_pwd(void)
{
	char *buf;

	buf = getcwd(NULL, 10);
	printf("%s\n", buf);
	free(buf);
}

void	bsh_env(void)
{
	char	**env;

	env = environ;
	while (*env)
	{
		if (ft_strchr(*env, '='))
			printf("%s\n", *env);
		env++;
	}	
}

static size_t	bsh_wordcount(char const *s, char c)
{
	int	wc;

	wc = 0;
	while (*s)
	{
		while (*s && !ft_memcmp(s, &c, 1))
			s++;
		if (*s)
		{
			if (*s == '"' && ft_strchr(s + 1, '"'))
				s = (ft_strchr(s + 1, '"')) + 1;
			if (*s == '\'' && ft_strchr(s + 1, '\''))
				s = (ft_strchr(s + 1, '\'')) + 1;
			wc++;
		}
		while (*s && ft_memcmp(s, &c, 1))
			s++;
	}
	return (wc);
}

char	**bsh_split(char const *s, char c)
{
	char	**arr;
	char	**a;
	size_t	size;

	if (!s)
		return (NULL);
	size = bsh_wordcount(s, c) + 1;
	arr = (char **)malloc(size * sizeof(char *));
	if (!arr)
		return (NULL);
	a = arr;
	while (*s)
	{
		while (*s && !ft_memcmp(s, &c, 1))
			s++;
		if (*s == '"' && ft_strchr(s + 1, '"'))
		{
			*a++ = ft_substr(s, 1, ft_strrchr(s, '"') - s - 1);
			s = ft_strchr(s + 1, '"') + 1;
		}
		if (*s == '\'' && ft_strchr(s + 1, '\''))
		{
			*a++ = ft_substr(s, 1, ft_strrchr(s, '\'') - s - 1);
			s = ft_strchr(s + 1, '\'') + 1;
		}
		if (*s && ft_strchr(s, c))
			*a++ = ft_substr(s, 0, ft_strchr(s, c) - s);
		else if (*s)
			*a++ = ft_substr(s, 0, ft_strchr(s, 0) - s);
		while (*s && ft_memcmp(s, &c, 1))
			s++;
	}
	*a = 0;
	return (arr);
}

int	is_token(char c)
{
	return (c == WHITE_SPACE || c == NEW_LINE || c == QUOTE
		|| c == DOUBLE_QUOTE || c == ESCAPE || c == ENV
		|| c == PIPE_LINE || c == REDIRECTION_IN
		|| c == REDIRECTION_OUT || c == WILDCARD
		|| c == PARENTASIS_OPEN || c == PARENTASIS_CLOSE);
}

t_linkedlist	*ft_lexer(char *line)
{
	t_linkedlist	*list;
	t_elem			elem;
	int				state;

	list = list_init(NULL);
	state = GENERAL;
	while (line && *line)
	{
		if (list->tail && (list->tail->type == QUOTE || list->tail->state == IN_QUOTE))
		{
			state = IN_QUOTE;
			if (*line == QUOTE)
				state = GENERAL;
		}
		else if (list->tail && (list->tail->type == DOUBLE_QUOTE || list->tail->state == IN_DOUBLE_QUOTE))
		{
			state = IN_DOUBLE_QUOTE;
			if (*line == DOUBLE_QUOTE)
				state = GENERAL;
		}
		else
			state = GENERAL;
		if (is_token(*line) && *line != ENV)
		{
			list_add_back(list, list_new_elem(line, 1, *line, state));
			line++;
		}
		else
		{
			elem.content = line;
			elem.len = 0;
			elem.type = WORD;
			if (*line == ENV)
			{
				elem.type = ENV;
			}
			elem.state = state;
			while (*line)
			{
				line++;
				elem.len++;
				if (is_token(*line))
					break ;
			}
			list_add_back(list, list_new_elem(elem.content, elem.len, elem.type, elem.state));
		}
	}
	return (list);
}

char	*state_to_text(int state)
{
	if (state == IN_DOUBLE_QUOTE)
		return ("IN_DOUBLE_QUOTE");
	if (state == IN_QUOTE)
		return ("IN_QUOTE");
	if (state == GENERAL)
		return ("GENERAL");
	if (state == IN_PARENTASIS)
		return ("IN_PARENTASIS");
	return (NULL);
}

char	*type_to_text(int type)
{
	if (type == WORD)
		return ("WORD");
	if (type == WHITE_SPACE)
		return ("WHITE_SPACE");
	if (type == NEW_LINE)
		return ("NEW_LINE");
	if (type == QUOTE)
		return ("QUOTE");
	if (type == DOUBLE_QUOTE)
		return ("DOUBLE_QUOTE");
	if (type == ESCAPE)
		return ("ESCAPE");
	if (type == ENV)
		return ("ENV");
	if (type == PIPE_LINE)
		return ("PIPE_LINE");
	if (type == REDIRECTION_IN)
		return ("REDIRECTION_IN");
	if (type == REDIRECTION_OUT)
		return ("REDIRECTION_OUT");
	if (type == HERE_DOC)
		return ("HERE_DOC");
	if (type == DOUBLE_REDIRECTION_OUT)
		return ("DOUBLE_REDIRECTION_OUT");
	if (type == WILDCARD)
		return ("WILDCARD");
	if (type == PARENTASIS_OPEN)
		return ("PARENTASIS_OPEN");
	if (type == PARENTASIS_CLOSE)
		return ("PARENTASIS_CLOSE");
	if (type == DOUBLE_AMPERSAND)
		return ("DOUBLE_AMPERSAND");
	if (type == LOGOCAL_OR_OPERATOR)
		return ("LOGOCAL_OR_OPERATOR");
	return (NULL);
}

void	print_linkedlist(t_linkedlist *list)
{
	t_elem	*elem;

	elem = list->head;
	printf("=================================================================\n");
	while (elem)
	{
		printf("content: %s\tlen: %d\tstate: %s\ttype: %s\n", elem->content, elem->len, state_to_text(elem->state), type_to_text(elem->type));
		elem = elem->next;
	}
	printf("=================================================================\n");
}

void	bash_promt(void)
{
	char	*line;

	line = readline(PROMPT_TEXT);
	if (!line)
		exit(!!printf("exit\n"));
	if (ft_strlen(line))
	{
		add_history(line);
		print_linkedlist(ft_lexer(line));
		// cmd = parse_command(line);
		// if (cmd.cmd && !ft_strncmp(cmd.cmd, "echo", ft_strlen(cmd.cmd)))
		// 	bsh_echo(&cmd);
		// if (cmd.cmd && !ft_strncmp(cmd.cmd, "cd", ft_strlen(cmd.cmd)))
		// 	bsh_chdir(&cmd);
		// if (cmd.cmd && !ft_strncmp(cmd.cmd, "pwd", ft_strlen(cmd.cmd)))
		// 	bsh_pwd();
		// if (cmd.cmd && !ft_strncmp(cmd.cmd, "env", ft_strlen(cmd.cmd)))
		// 	bsh_env();
		// if (cmd.cmd && !ft_strncmp(cmd.cmd, "exit", ft_strlen(cmd.cmd)))
		// 	exit(!printf("exit\n"));
		// if (cmd.cmd && !ft_strncmp(cmd.cmd, "dbg", ft_strlen(cmd.cmd)))
		// 	get_env_variable(cmd.args[1]);
	}
	free(line);
}

void	sig_ign_handler(int signum)
{
	(void)signum;
	rl_on_new_line();
	write(1, "\n", 1);
	rl_replace_line("", 1);
	rl_redisplay();
}

void	disable_sigint_char(void)
{
	struct termios	t;

	tcgetattr(STDIN_FILENO, &t);
	t.c_cflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void	enable_sigint_char(void)
{
	struct termios	t;

	tcgetattr(STDIN_FILENO, &t);
	t.c_cflag |= ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	(void)envp;
	disable_sigint_char();
	// enable_sigint_char();
	while (1)
	{
		signal(SIGINT, sig_ign_handler);
		signal(SIGQUIT, SIG_IGN);
		bash_promt();
	}
}
