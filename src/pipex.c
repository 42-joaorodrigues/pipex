/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-alm <joao-alm@student.42luxembourg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 19:51:19 by joao-alm          #+#    #+#             */
/*   Updated: 2025/10/19 00:25:11 by joao-alm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void	exec_cmd(char *cmd, char **envp);

void	close_fds(int fds[2][2])
{
	close(fds[0][0]);
	close(fds[0][1]);
	close(fds[1][0]);
	close(fds[1][1]);
}

void	open_fds(char **av, int fds[2][2])
{
	fds[0][0] = open(av[1], O_RDONLY);
	if (fds[0][0] < 0)
	{
		perror("open infile");
		exit(1);
	}
	fds[0][1] = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fds[0][1] < 0)
	{
		close(fds[0][0]);
		perror("open outfile");
		exit(1);
	}
	if (pipe(fds[1]) < 0)
	{
		close(fds[0][0]);
		close(fds[0][1]);
		perror("pipe");
		exit(1);
	}
}

void	execute_last_cmd(char *cmd, char **envp, int fds[2][2], int *pids)
{
	if (pids[1] < 0)
	{
		close_fds(fds);
		perror("fork");
		exit(1);
	}
	if (pids[1] == 0)
	{
		dup2(fds[1][0], STDIN_FILENO);
		dup2(fds[0][1], STDOUT_FILENO);
		close_fds(fds);
		exec_cmd(cmd, envp);
	}
}

void	execute_first_cmd(char *cmd, char **envp, int fds[2][2], int *pids)
{
	if (pids[0] < 0)
	{
		close_fds(fds);
		perror("fork");
		exit(1);
	}
	if (pids[0] == 0)
	{
		dup2(fds[0][0], STDIN_FILENO);
		dup2(fds[1][1], STDOUT_FILENO);
		close_fds(fds);
		exec_cmd(cmd, envp);
	}
	pids[1] = fork();
}

int	main(int ac, char **av, char **envp)
{
	int		fds[2][2];
	pid_t	pids[2];
	int		status;

	if (ac != 5)
	{
		write(2, "Error: Invalid Arguments\n", 25);
		return (1);
	}
	open_fds(av, fds);
	pids[0] = fork();
	execute_first_cmd(av[2], envp, fds, pids);
	execute_last_cmd(av[3], envp, fds, pids);
	close_fds(fds);
	waitpid(pids[0], NULL, 0);
	waitpid(pids[1], &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
