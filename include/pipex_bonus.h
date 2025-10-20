/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-alm <joao-alm@student.42luxembourg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 18:26:36 by joao-alm          #+#    #+#             */
/*   Updated: 2025/10/20 00:53:17 by joao-alm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include <stdbool.h>
# include <sys/wait.h>

typedef int	t_pipe[2];

typedef struct s_pipex
{
	bool	heredoc;
	int		fd_in;
	int		fd_out;
	int		n_cmd;
	t_pipe	*pipes;
	pid_t	*pids;
}				t_pipex;

// exec
void	exec_cmd(char *cmd, char **envp);

// util
int		init(int ac, char **av, t_pipex *pp);
void	close_fds(t_pipex *pp);
void	clean_exit(t_pipex *pp, char *errmsg);
int		handle_heredoc(char *delimiter);

#endif
