#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

/**
 * Check if a path exists.
 *
 * @param path The path to check.
 *
 * @return 1 if the path exists, 0 otherwise.
 */
int path_exists(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == 0)
        return 1;
    else
    {
        return 0;
    }
}

/**
 * @brief check if a path is a file
 *
 * @param path The path to check
 * @return int 1 if the path is a file, 0 otherwise
 */
int path_is_file(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == -1)
    {
        return 0;
    }
    else
    {
        return S_ISREG(statbuf.st_mode);
    }
}

/**
 * @brief check if a path is a directory
 *
 * @param path The path to check
 * @return int 1 if the path is a directory, 0 otherwise
 */
int path_is_directory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == -1)
    {
        return 0;
    }
    else
    {
        return S_ISDIR(statbuf.st_mode);
    }
}

/**
 * Compare the contents of two files.
 *
 * @param file1 The path to the first file.
 * @param file2 The path to the second file.
 *
 * @return 0 if the files are identical, 1 otherwise.
 */
int compare_files(const char *file1, const char *file2)
{
    // try to open the files
    int fd1 = open(file1, O_RDONLY);
    if (fd1 == -1)
    {
        perror(file1);
        return 1;
    }
    int fd2 = open(file2, O_RDONLY);
    if (fd2 == -1)
    {
        perror(file2);
        close(fd1);
        return 1;
    }

    char buf1[BUFSIZ], buf2[BUFSIZ];
    ssize_t n1, n2;

    // compare them until we reach the end of either
    while (1)
    {
        n1 = read(fd1, buf1, BUFSIZ);
        n2 = read(fd2, buf2, BUFSIZ);

        if (n1 == 0 && n2 == 0)
        {
            close(fd1);
            close(fd2);
            return 0;
        }
        if (n1 < 0 || n2 < 0 || n1 != n2 || memcmp(buf1, buf2, n1) != 0)
        {
            close(fd1);
            close(fd2);
            return 1;
        }
    }
}

void merge_files(char *file1, char *file2)
{
    struct stat stat1, stat2;

    // Check if files are the same
    if (stat(file1, &stat1) == -1)
    {
        perror(file1);
        return;
    }

    if (stat(file2, &stat2) == -1)
    {
        perror(file2);
        return;
    }

    // check if files are same hard links
    if (stat1.st_ino == stat2.st_ino)
    {
        printf("%s and %s are the same file\n", file1, file2);
        return;
    }

    // Check if files are the same size
    if (stat1.st_size != stat2.st_size)
    {
        printf("%s and %s are different sizes\n", file1, file2);
        return;
    }
    else
    {
        if (compare_files(file1, file2) != 0)
        {
            printf("%s and %s are different files\n", file1, file2);
            return;
        }
        else // two files are the same
        {
            if (unlink(file2) == -1)
            {
                perror("unlink");
                return;
            }
            if (link(file1, file2) == -1)
            {
                perror("link");
                return;
            }

            printf("%s and %s are the same file, linked\n", file1, file2);
        }
    }
}

void merge_directories(char *dir1, char *dir2)
{
    DIR *dp1;
    struct dirent *entry1;
    char path1[PATH_MAX], path2[PATH_MAX];

    // try to open the directories
    dp1 = opendir((const char *)dir1);
    if (dp1 == NULL)
    {
        perror(dir1);
        return;
    }

    while ((entry1 = readdir(dp1)) != NULL)
    {
        // skip hidden files
        if (entry1->d_name[0] == '.')
        {
            continue;
        }

        snprintf(path1, PATH_MAX, "%s/%s", dir1, entry1->d_name);
        snprintf(path2, PATH_MAX, "%s/%s", dir2, entry1->d_name);

        // check if path2 exists and is a file
        if (path_is_file(path1))
        {
            if (path_is_file(path2))
            {
                merge_files(path1, path2);
                continue;
            }
        }
        // check if path2 exists and is a directory
        else if (path_is_directory(path1))
        {
            if (path_is_directory(path2))
            {
                merge_directories(path1, path2);
                continue;
            }
        }

        if (closedir(dp1) == -1)
            perror("closedir");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s file1 file2 or %s dir1 dir2\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat stat1, stat2;

    // Check if files exist
    if (stat(argv[1], &stat1) == -1)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    if (stat(argv[2], &stat2) == -1)
    {
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }

    // Check if files are the same type
    if (S_ISREG(stat1.st_mode) && S_ISREG(stat2.st_mode))
    {
        merge_files(argv[1], argv[2]);
    }
    else if (S_ISDIR(stat1.st_mode) && S_ISDIR(stat2.st_mode))
    {
        merge_directories(argv[1], argv[2]);
    }
    else
    {
        printf("%s and %s are different types\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    return 0;
}