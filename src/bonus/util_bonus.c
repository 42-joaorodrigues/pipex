/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-alm <joao-alm@student.42luxembourg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 18:12:45 by joao-alm          #+#    #+#             */
/*   Updated: 2025/10/20 01:12:45 by joao-alm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex_bonus.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int	init(int ac, char **av, t_pipex *pp)
{
	int	i;

	pp->heredoc = false;
	if (ft_strlen(av[1]) == 8 && ft_strncmp(av[1], "here_doc", 8) == 0)
		pp->heredoc = true;
	pp->fd_in = -1;
	pp->fd_out = -1;
	pp->n_cmd = ac - 3;
	if (pp->heredoc)
		pp->n_cmd--;
	pp->pipes = malloc(sizeof(t_pipe) * (pp->n_cmd - 1));
	if (!pp->pipes)
		return (write(2, "error: memory allocation failed\n", 25), 1);
	i = -1;
	while (++i < pp->n_cmd - 1)
	{
		pp->pipes[i][0] = -1;
		pp->pipes[i][1] = -1;
	}
	pp->pids = malloc(sizeof(pid_t) * pp->n_cmd);
	if (!pp->pids)
		return (free(pp->pipes), write(2,
				"error: memory allocation failed\n", 25), 1);
	return (0);
}

void	close_fds(t_pipex *pp)
{
	int	i;

	if (pp->fd_in > 0)
		close(pp->fd_in);
	if (pp->fd_out > 0)
		close(pp->fd_out);
	i = -1;
	while (++i < pp->n_cmd - 1)
	{
		if (pp->pipes[i][0] > 0)
			close(pp->pipes[i][0]);
		if (pp->pipes[i][1] > 0)
			close(pp->pipes[i][1]);
	}
}

void	clean_exit(t_pipex *pp, char *errmsg)
{
	if (errmsg)
		perror(errmsg);
	close_fds(pp);
	free(pp->pipes);
	free(pp->pids);
	exit(1);
}

static int	heredoc_write(char *line, t_pipe p)
{
	ssize_t	llen;
	ssize_t	w;
	ssize_t	off;

	llen = ft_strlen(line);
	w = 0;
	off = 0;
	while (off < llen)
	{
		w = write(p[1], line + off, llen - off);
		if (w <= 0)
		{
			free(line);
			close(p[0]);
			close(p[1]);
			return (-1);
		}
		off += w;
	}
	return (0);
}

int	handle_heredoc(char *delimiter)
{
	t_pipe	p;
	char	*line;
	
	if (pipe(p) < 0)
		return (-1);
	while (1)
	{
		line = get_next_line(STDIN_FILENO);
		if (!line)
		{
			errno = ECANCELED;
			return (close(p[0]), close(p[1]), -1);
		}
		if (ft_strlen(line) - 1 == ft_strlen(delimiter)
			&& ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
			break ;
		if (heredoc_write(line, p) != 0)
			return (-1);
		free(line);
	}
	free(line);
	close(p[1]);
	return (p[0]);	
}
