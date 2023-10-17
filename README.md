# merge_ln

Merge two files or two directories by creating hard links to identical files

## Usage

The `merge_ln.sh` script is used to test if two files are the same and merge them
as hard links if they are. The script can take either two file names or two directory
names as arguments, and will merge the files in the directories iteratively.

To use the script, run:

```bash
merge_ln file1 file2
```

or

```bash
merge_ln dir1 dir2
```

Replace `file1`, `file2`, `dir1`, and `dir2` with the names of the files and directories you want to merge.

## Installation

To install or upgrade the script, run:

```bash
sudo wget -O /usr/local/bin/merge_ln https://raw.githubusercontent.com/tanganke/merge_ln/main/merge_ln.sh && sudo chmod +x /usr/local/bin/merge_ln
```

## Uninstallation

To uninstall the script, run:

```bash
sudo rm /usr/local/bin/merge_ln
```

## Description

The `merge_ln.sh` script defines two functions: merge_files and merge_directories.

The merge_files function takes two file names as arguments and uses the `cmp` and `ln` commands to merge the files
if they are the same. The merge_directories function takes two directory names as arguments and iteratively merges
the files in the directories using the merge_files function.

The script then checks if the arguments are files or directories, and calls the appropriate function. If the
arguments are not files or directories, the script prints a usage message.

The script uses hard links to merge the files, which means that the files will share the same inode and data
blocks on the file system. This can save disk space and improve performance, since the files will not be duplicated.
However, it also means that *changes to one file will affect the other file, since they are essentially the same file*.

Note that the script does not handle symbolic links or other types of files. It also does not handle cases
where the same file name appears in both directories but the files are different.
