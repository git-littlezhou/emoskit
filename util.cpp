/*
*
* Copyright 2017 emsokit authors.
* All rights reserved.
*
* Licensed under the BSD 3-Clause License (the "License"); you may
* not use this file except in compliance with the License. You may
* obtain a copy of the License at
*
*	https://opensource.org/licenses/BSD-3-Clause
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
* implied. See the License for the specific language governing
* permissions and limitations under the License.
*
*/

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>

#include "util.h"

namespace emoskit {

	namespace util {

		int noinline_
		CreateSocketAndListen(const char * host, const char * serv, socklen_t * addrlen){

			int			listenfd, n;
			const int		on = 1;
			struct addrinfo	hints, *res, *ressave;

			const int LISTENQ = 1024;

			bzero(&hints, sizeof(struct addrinfo));
			hints.ai_flags = AI_PASSIVE;
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;

			/*get server address info*/
			if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
				emoskit_log(ERROR_LOG, "tcp error for %s,%s: %s", host, serv, gai_strerror(n));

			ressave = res;

			do {
				listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
				if (listenfd < 0)
					continue;

				setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
				if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
					break;

				// bind error, close listenfd and try next address
				close(listenfd);
			} while ((res = res->ai_next) != NULL);

			if (res == NULL) {
				emoskit_log(ERROR_LOG, "tcp error for %s:%s, errno: %d, %s", host, serv, errno, std::strerror(errno));
				freeaddrinfo(ressave);
				return -1;
			}

			listen(listenfd, LISTENQ);

			if (addrlen)
				*addrlen = res->ai_addrlen;

			freeaddrinfo(ressave);

			return(listenfd);
		}

		int noinline_ 
		CreateSocketAndConnect(const char* ip, unsigned port, int timeout_ms, const char* bind_ip, int bind_port) {
			
			int sockfd = -1;
			struct sockaddr_in servaddr;

			if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
				emoskit_log(ERROR_LOG, "invalid ip: %s", ip);
				return sockfd;
			}

			if (bind_ip != nullptr && inet_pton(AF_INET, bind_ip, &servaddr.sin_addr) <=0) {
				emoskit_log(ERROR_LOG, "invalid bind_ip: %s", bind_ip);
				return sockfd;
			}

			sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

			if (sockfd < 0) {
				emoskit_log(ERROR_LOG, "create socket fail, errno: %d, %s", errno, std::strerror(errno));
				return sockfd;
			}

			memset(&servaddr, 0, sizeof(servaddr));

			servaddr.sin_family = AF_INET;

			/* if bind ip is set, try to bind to the specified address */
			if (bind_ip != nullptr) {
				inet_pton(AF_INET, bind_ip, &servaddr.sin_addr);
				servaddr.sin_port = htons(bind_port);
				if (::bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
					emoskit_log(WARN_LOG, "bind to the [%s:%d] fail, errno: %d, %s", bind_ip, bind_port, errno, std::strerror(errno));
				}
			}

			inet_pton(AF_INET, ip, &servaddr.sin_addr);
			servaddr.sin_port = htons(port);

			int flags = fcntl(sockfd, F_GETFL, 0);
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

			int ret = connect(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr));

			if (ret < 0 && errno != EINPROGRESS) {
				close(sockfd);
				emoskit_log(ERROR_LOG, "connect to remote host [%s:%d] fail, errno: %d, %s", ip, port, errno, std::strerror(errno));
				return -1;
			}

			int error = 0;
			
			if (ret != 0) {
				fd_set rset, wset;
				struct timeval tval;

				FD_ZERO(&rset);
				FD_SET(sockfd, &rset);
				wset = rset;

				tval.tv_sec = timeout_ms / 1000;
				tval.tv_usec = (timeout_ms % 1000) * 1000;
				
				for (int i = 0; i < 3; ++i) {
					ret = select(sockfd + 1, &rset, &wset, nullptr, timeout_ms > 0 ? &tval : nullptr);
					if (-1 == ret && errno == EINTR)
						continue;
					else break;
				}

				if (0 == ret) {
					close(sockfd);
					errno = ETIMEDOUT;
					return -1;
				}

				if (ret < 0) {
					emoskit_log(ERROR_LOG, "select error, errno: %d, %s", errno, std::strerror(errno));
					close(sockfd);
					return -1;
				}

				if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
					socklen_t len = sizeof(error);
					if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
						emoskit_log(ERROR_LOG, "getsockopt error, errno: %d, %s", errno, std::strerror(errno));
						error = -1;
					}
				}
				else {
					emoskit_log(ERROR_LOG, "select error, sockfd not set");
					error = -1;
				}

			}

			fcntl(sockfd, F_SETFL, flags);

			if (error != 0) {
				close(sockfd);
				/* getsockopt return some sockfd error */
				if (error != -1) {
					errno = error;
					emoskit_log(ERROR_LOG, "connect to remote host [%s:%d] fail, errno: %d, %s", ip, port, errno, std::strerror(errno));
				}
				return -1;
			}

			return sockfd;
		}

		bool noinline_ 
		ReadFile(const char* file_path, std::string* contents) {

			char new_file_path[256] = { 0 };

			if ('~' == file_path[0])
				std::snprintf(new_file_path, sizeof(new_file_path), "%s%s", std::getenv("HOME"), file_path + 1);
			else
				std::snprintf(new_file_path, sizeof(new_file_path), "%s", file_path);

			util_fd_t file_fd = ::open(new_file_path, O_RDONLY);

			bool succeed_or_not = false;

			if (file_fd > 0) {
				struct stat st;
				if (0 == ::fstat(file_fd, &st)) {

						contents->resize(st.st_size);

					if (st.st_size == ::read(file_fd, (void*)contents->data(), st.st_size))
						succeed_or_not = true;
					else
						emoskit_log(ERROR_LOG, "read file %s fail, errno: %d, %s", new_file_path, errno, std::strerror(errno));
				}
				else {
					emoskit_log(ERROR_LOG, "get file %s stat fail, errno: %d, %s", new_file_path, errno, std::strerror(errno));
				}

				close(file_fd);
			}
			else {
				emoskit_log(ERROR_LOG, "open %s fail, errno: %d, %s", new_file_path, errno, std::strerror(errno));
			}

			return succeed_or_not;
		}

		void Daemonize() {
			int fd;

			// parent exits
			if (fork() != 0) exit(0); 

			// create a new session
			setsid();

			/* Every output is dupped to /dev/null. If emoskit is daemonized but
			 * the 'logfile' is set to 'stdout' in the configuration file
			 * it will not log at all. */
			if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
				dup2(fd, STDIN_FILENO);
				dup2(fd, STDOUT_FILENO);
				dup2(fd, STDERR_FILENO);
				if (fd > STDERR_FILENO) close(fd);
			}
		}
	}
}