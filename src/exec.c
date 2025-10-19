/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-alm <joao-alm@student.42luxembourg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 19:49:11 by joao-alm          #+#    #+#             */
/*   Updated: 2025/10/20 01:31:44 by joao-alm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char	**get_argv(char *cmd)
{
	char	**argv;

	argv = ft_split(cmd, ' ');
	if (!argv)
	{
		write(2, "error: memory allocation failed\n", 32);
		exit(127);
	}
	if (!argv[0])
	{
		free_split(argv);
		errno = EINVAL;
		perror("parse");
		exit(127);
	}
	return (argv);
}

char	*get_env_path(char **envp)
{
	int	i;

	if (!envp)
		return (NULL);
	i = -1;
	while (envp[++i])
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
	return (NULL);
}

static char	*get_full_path(char **paths, char **argv)
{
	char	*full_path;

	while (*paths)
	{
		full_path = malloc(strlen(*paths) + 1 + strlen(argv[0]) + 1);
		if (!full_path)
		{
			write(2, "error: memory allocation failed\n", 32);
			free_split(argv);
			free_split(paths);
			exit(127);
		}
		ft_strcpy(full_path, *paths++);
		ft_strcat(full_path, "/");
		ft_strcat(full_path, argv[0]);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
	}
	return (argv[0]);
}

char	**get_paths(char **argv, char **envp)
{
	char	*env_path;
	char	**paths;

	env_path = get_env_path(envp);
	if (!env_path)
	{
		write(2, "error: PATH not set\n", 20);
		free_split(argv);
		exit(127);
	}
	paths = ft_split(env_path, ':');
	if (!paths)
	{
		write(2, "error: memory allocation failed\n", 32);
		free_split(argv);
		exit(127);
	}
	return (paths);
}

void	exec_cmd(char *cmd, char **envp)
{
	char	**argv;
	char	**paths;
	char	*full_path;

	argv = get_argv(cmd);
	if (ft_strchr(argv[0], '/'))
	{
		execve(argv[0], argv, envp);
		perror(argv[0]);
		free_split(argv);
		exit(127);
	}
	paths = get_paths(argv, envp);
	full_path = get_full_path(paths, argv);
	execve(full_path, argv, envp);
	perror(argv[0]);
	if (full_path != argv[0])
		free(full_path);
	free_split(argv);
	free_split(paths);
}
