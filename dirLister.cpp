/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dirLister.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bgannoun <bgannoun@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 16:04:13 by bgannoun          #+#    #+#             */
/*   Updated: 2024/06/10 16:24:02 by bgannoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
  DIR *dir;
  struct dirent *entry;

  // Open the current directory (you can replace "." with a specific path)
  dir = opendir(".");
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  // Read entries from the directory
  while ((entry = readdir(dir)) != NULL) {
    // Skip special entries "." and ".."
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
	struct stat statbuf;
	if (stat(entry->d_name, &statbuf) == 0){
		if (S_ISREG(statbuf.st_mode)){
			printf("%s (regular file)\n", entry->d_name);
		}
		else if (S_ISDIR(statbuf.st_mode)){
			printf("%s (directory)\n", entry->d_name);
		}
	}
  }

  // Close the directory stream
  closedir(dir);

  return 0;
}
