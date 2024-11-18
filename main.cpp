/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bramzil <bramzil@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 13:11:44 by bramzil           #+#    #+#             */
/*   Updated: 2024/11/18 19:30:48 by bramzil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "main.hpp"

static void init_socket_addr(int AF, sockaddr *s_addr, int port)
{
    sockaddr_in  *in_addr;
    if (AF == AF_INET)
    {
        in_addr = (sockaddr_in*)s_addr;
        memset(in_addr, 0, sizeof(sockaddr_in));
        in_addr->sin_family = AF_INET;
        in_addr->sin_port = htons(port);
        in_addr->sin_len = sizeof(sockaddr_in);
    }
}

static int script_executer(int s_fd)
{
    int pid;
    char *arv[] = { "/usr/bin/php", "./greating.php", NULL};
    char *env[] = { "REQUEST_METHOD=GET",
                    "SCRIPT_NAME=greating.php",
                    "SERVER_PROTOCOL=HTTP/1.1",
                    "QUERY_STRING=fname=khalid&lname=lmqari",
                    "CONTENT_TYPE=text/html; UTF-8",
                    NULL};

    if ((pid = fork()) < 0)
        return (write(2, "fork fails!!\n", 14), -1);
    else if (pid == 0)
    {
        if ((dup2(s_fd, 1) < 0) || (dup2(s_fd, 0) < 0))
        {
            write(2, "dup2 fails!!\n", 14);
            exit(1);
        }
        else if (execve("/usr/bin/php", arv, env) < 0)
        {
            write(2, "execuve fails!!\n", 17);
            exit(1);
        }    
    }
    return (pid);
}

static int monitor_child(int pid, int h_fd, int c_fd)
{
    char buf[512];
    int st, stat, r_byts;
    
    st = waitpid(pid, &stat, WNOHANG);
    if (st < 0)
        return (write(2, "there's no child process!!\n", 28), -1);
    else if (st == 0)
    {
        while (0 <= (r_byts = read(h_fd, buf, 512)))
            write(1, buf, r_byts);
        return (0);
    }
    return (st);
}

int main(int ac, char **arv, char **env)
{
    socklen_t       sock_len;
    sockaddr_in     sr_inaddr;
    std::vector<std::string> headers;
    char            ipstr[INET_ADDRSTRLEN];
    int             s_fd, c_fd, p_fd[2], r_bytes, opt, pid;

    opt = 1;
    init_socket_addr(AF_INET, (sockaddr*)&sr_inaddr, 8080);
    if (inet_pton(AF_INET, "127.0.0.1", &sr_inaddr.sin_addr) < 0)
        return (write(2, "inet_pton fails!!\n", 19));
    else if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return (write(2, "socket fails!!\n", 16));
    else if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &sr_inaddr, sizeof(sr_inaddr)) < 0)
        return (close(s_fd), write(2, "setsockopt fails!!\n", 20));
    else if (bind(s_fd, (sockaddr*)&sr_inaddr, sizeof(sr_inaddr)) < 0)
        return (close(s_fd), write(2, "bind fails!!\n", 14));
    else if (listen(s_fd, 5) < 0)
        return (close(s_fd), write(2, "listen fails!!\n", 16));
    else
    {
        if ((c_fd = accept(s_fd, NULL, NULL)) < 0)
            return (close(s_fd), write(2, "accept fails!!\n", 16));
        else if (socketpair(AF_UNIX, SOCK_STREAM, 0, p_fd) < 0)
            return (close(s_fd), write(2, "socketpair fails!!\n", 20));
        else if ((pid = script_executer(p_fd[1])) < 0)
            return (close(s_fd), close(p_fd[0]), close(p_fd[1]), 1);
        monitor_child(pid, p_fd[0], c_fd);
        close(c_fd);
    }
    close(s_fd);
    return (0);
}