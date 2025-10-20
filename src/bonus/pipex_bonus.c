/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-alm <joao-alm@student.42luxembourg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 19:51:19 by joao-alm          #+#    #+#             */
/*   Updated: 2025/10/20 01:28:28 by joao-alm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void	open_fds(int ac, char **av, t_pipex *pp)
{
	int	i;

	if (pp->heredoc)
		pp->fd_in = handle_heredoc(av[2]);
	else
		pp->fd_in = open(av[1], O_RDONLY);
	if (pp->fd_in < 0)
		clean_exit(pp, av[1]);
	if (pp->heredoc)
		pp->fd_out = open(av[ac - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		pp->fd_out = open(av[ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pp->fd_out < 0)
		clean_exit(pp, av[ac - 1]);
	i = -1;
	while (++i < pp->n_cmd - 1)
		if (pipe(pp->pipes[i]) < 0)
			clean_exit(pp, "pipe");
}

void	handle_cmd(char **av, char **envp, t_pipex *pp, int i)
{
	if (i == 0)
		dup2(pp->fd_in, STDIN_FILENO);
	else
		dup2(pp->pipes[i - 1][0], STDIN_FILENO);
	if (i == pp->n_cmd - 1)
		dup2(pp->fd_out, STDOUT_FILENO);
	else
		dup2(pp->pipes[i][1], STDOUT_FILENO);
	close_fds(pp);
	exec_cmd(av[i], envp);
	free(pp->pipes);
	free(pp->pids);
	exit(127);
}

void	fork_and_execute(char **av, char **envp, t_pipex *pp)
{
	int		i;

	i = -1;
	while (++i < pp->n_cmd)
	{
		pp->pids[i] = fork();
		if (pp->pids[i] < 0)
		{
			perror("fork");
			close_fds(pp);
			while (--i >= 0)
				waitpid(pp->pids[i], NULL, 0);
			close_fds(pp);
			free(pp->pids);
			exit(1);
		}
		if (pp->pids[i] == 0)
			handle_cmd(av, envp, pp, i);
	}
}

int	wait_and_clean(t_pipex *pp)
{
	int	status;
	int	ret;
	int	i;

	close_fds(pp);
	i = -1;
	while (++i < pp->n_cmd)
	{
		waitpid(pp->pids[i], &status, 0);
		if (WIFEXITED(status))
			ret = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			ret = 128 + WTERMSIG(status);
		else
			ret = 1;
	}
	free(pp->pipes);
	free(pp->pids);
	return (ret);
}

int	main(int ac, char **av, char **envp)
{
	t_pipex	pipex;

	if (ac < 5)
	{
		write(2, "error: invalid arguments\n", 25);
		return (1);
	}
	if (init(ac, av, &pipex) != 0)
		return (1);
	open_fds(ac, av, &pipex);
	av += 2;
	if (pipex.heredoc)
		av++;
	fork_and_execute(av, envp, &pipex);
	return (wait_and_clean(&pipex));
}
