# What is fastcd ?
- Search for files/folders , it becomes very easy and quick
- You can search cpp/c definitions in VIM with using **fcd.vim**

# About Code
The code is ugly , and just quick and dirty work for myself.

# Usage
**It supports two usage ：**
- **command-line**
- **command-view : Need Install VIM**
###command-line : 
**In terminal . Input “fcd -h” And Press the Enter key**
> ```
> $user:~> fcd -h
> 
> usage fcd [ l | a | d | e | index ]
> l $line(d:line=10) : list
> p $index : print the path of index
> flash $path(d:path=$HOME) : refresh file index 
> index : fast cd to $index path
> key : will search $key and cd path if only one matched
> search_define key top-file sample-line max-depth a->b 
> magic : [-v reverse] [-i casecmp] [-r restart] [:index] 
> 
> ```

###command-view :
####1: In terminal . Input "fcd" And Press the Enter key
```
$user:~> fcd
```
####2: Input your key-word for searching file/folder
For Example , I want to search the file which called **/home/xxx-user/a/b/c/targer**
> ```
> targer //<-- you Input "targer" and Press the Enter key
> ---------------------------------------------------------
> /home/xxx-user/a/b/c/targ [0]
> /home/xxx-user/a/b/c/targer [1]
> /home/xxx-user/a/b/c/targer-fake [2]
> /home/xxx-user/a/b/c/targer-dummy [3]
> ```
####3: Input The Index For Select your target
For Example , My targer-file is 
```
/home/xxx-user/a/b/c/targer [1]
```
So 
> ```
> :1 //<-- you Input ":1" and Press the Enter key
> ---------------------------------------------------------
> /home/xxx-user/a/b/c/targer [1]
> ```

# Install
###1：Compiled code
**make and copy binary into "~/tools/fcd.app"**

> \$user:~> cd fastcd/ && make
> g++ -Wall -g fastcd.cpp -o fastcd.out -pthread -lncurses
> 
> \$user:~> cp fastcd.out ~/tools/fcd.app
> 

###2: Configure
**vim .bashrc/.profile , Add : **
> export PATH=/home/phoneli/tools:$PATH

